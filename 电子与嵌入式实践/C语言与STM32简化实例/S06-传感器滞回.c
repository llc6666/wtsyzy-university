/* S06 传感器阈值控制与滞回：从 DO 直通到软件滞回
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic sensor-hysteresis-demo.c -o sensor-hysteresis-demo.exe
 *
 * 目的：把「直通控制」「滞回（施密特）」「报警持续去抖」三个层次跑通并对比。
 * 模拟方式：一组光敏 AO/ADC 读数序列（值越大越暗，含 999/1001 边界抖动），
 *           三种控制策略各跑一遍，打印逐采样对照表与翻转次数统计。
 * 对照场景：数字量直通 LED 与模拟量阈值控制的差别：
 *       阈值由模块电位器硬件决定，PB12 输出、PB13 输入）。
 *       本 demo 模拟的是它下一步：AO 接 ADC、软件里定阈值的三种写法。
 */

#include <stdio.h>
#include <stdint.h>

/* ---- 模拟光敏 AO/ADC 读数：值越大越暗，12 位 ADC 值范围 ----
 * 序列设计：缓慢变暗到 999/1001 边界抖动区，再彻底变暗，然后缓慢变亮，
 *           在 799/801 附近再次抖动。 */
static const uint16_t light_samples[] = {
    800, 850, 900, 950, 999, 1001, 999, 1001, 1000, 1050, 1100,
    1000, 950, 900, 850, 799, 801, 799, 750, 700
};
#define N_SAMPLES ((int)(sizeof(light_samples) / sizeof(light_samples[0])))

#define ACTION_LEVEL  1000     /* 动作值：比这暗就开灯/报警 */
#define RELEASE_LEVEL  800     /* 释放值：比这亮才关灯（滞回带 200） */

/* 策略 1：直通——读数超过阈值就开，否则关 */
static uint8_t control_direct(uint16_t light)
{
    return light > ACTION_LEVEL ? 1u : 0u;
}

/* 策略 2：滞回——状态记忆。从关到开要过动作值，从开到关要低于释放值 */
static uint8_t control_hysteresis(uint16_t light, uint8_t state)
{
    if (!state && light > ACTION_LEVEL) {
        return 1u;                     /* 关 -> 开：比动作值暗 */
    }
    if (state && light < RELEASE_LEVEL) {
        return 0u;                     /* 开 -> 关：比释放值亮 */
    }
    return state;                      /* 带内：维持原状态 */
}

/* 策略 3：滞回 + 持续去抖——报警信号需连续 N_STABLE 个采样成立才真正切换 */
#define N_STABLE 3

static uint8_t control_debounced(uint8_t raw, uint8_t *state, uint8_t *counter)
{
    if (raw != *state) {
        (*counter)++;
        if (*counter >= N_STABLE) {    /* 连续 3 个采样都要求切换 */
            *state = raw;
            *counter = 0;
        }
    } else {
        *counter = 0;                  /* 要求撤销，清零重新数 */
    }
    return *state;
}

int main(void)
{
    uint8_t d_state = 0, h_state = 0, db_state = 0, db_raw = 0, db_counter = 0;
    int d_flips = 0, h_flips = 0, db_flips = 0;
    uint8_t prev_d = 0, prev_h = 0, prev_db = 0;
    int i;

    printf("光敏 AO/ADC 读数序列（值越大越暗），动作值 %d，滞回释放值 %d\n\n",
           ACTION_LEVEL, RELEASE_LEVEL);
    printf("采样  读数   直通LED  滞回LED  滞回+去抖LED\n");
    printf("----  -----  -------  -------  -----------\n");

    for (i = 0; i < N_SAMPLES; i++) {
        uint16_t light = light_samples[i];

        d_state = control_direct(light);
        h_state = control_hysteresis(light, h_state);
        db_raw = control_hysteresis(light, db_raw);          /* 内层仍用滞回 */
        db_state = control_debounced(db_raw, &db_state, &db_counter);

        printf("%4d  %5u    %s      %s       %s\n",
               i, (unsigned)light,
               d_state ? "开" : "关",
               h_state ? "开" : "关",
               db_state ? "开" : "关");

        if (i > 0) {
            if (d_state != prev_d) d_flips++;
            if (h_state != prev_h) h_flips++;
            if (db_state != prev_db) db_flips++;
        }
        prev_d = d_state; prev_h = h_state; prev_db = db_state;
    }

    printf("\n翻转次数：直通 %d 次，滞回 %d 次，滞回+去抖 %d 次\n",
           d_flips, h_flips, db_flips);
    printf("直通在 999/1001 边界抖动段来回开关；滞回一旦开了，要亮回 800 以下才关。\n");
    printf("去抖版再多一道：状态要连续 %d 个采样都要求切换才动，毛刺全部滤掉。\n",
           N_STABLE);

    return 0;
}
