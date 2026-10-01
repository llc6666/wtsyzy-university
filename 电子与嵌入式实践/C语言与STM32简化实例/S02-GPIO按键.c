/* S02 GPIO 与按键：寄存器读写 + 位操作 + 消抖状态机
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic gpio-key-demo.c -o gpio-key-demo.exe
 *
 * 目的：把「读寄存器某一位」「改寄存器某几位」「用状态机滤掉抖动」三件事跑通。
 * 模拟方式：用普通变量当寄存器，用一组预设的电平序列当按键波形（含抖动）。
 */

#include <stdio.h>
#include <stdint.h>

/* ---- 模拟一组 GPIO 寄存器（真实硬件上是固定地址） ---- */
typedef struct {
    volatile uint32_t CRL;   /* 配置低寄存器 */
    volatile uint32_t CRH;
    volatile uint32_t IDR;   /* 输入数据寄存器 */
    volatile uint32_t ODR;   /* 输出数据寄存器 */
} gpio_t;

static gpio_t gpioa;                    /* 当作 GPIOA */

#define PIN_LED  (1u << 5)              /* PA5 接 LED */
#define PIN_KEY  (1u << 0)              /* PA0 接按键（低电平按下） */

/* 把 PA5 配置成输出：CRL 里每个引脚占 4 位，先清后写 */
static void config_led_output(void)
{
    const uint32_t shift = 4u * 5u;
    gpioa.CRL = (gpioa.CRL & ~(0x0Fu << shift)) | (0x03u << shift);
}

/* 读按键电平：0 表示按下 */
static uint8_t key_level(void)
{
    return (gpioa.IDR & PIN_KEY) ? 1u : 0u;
}

/* LED 翻转：只对目标位取反，不动别的位 */
static void led_toggle(void)
{
    gpioa.ODR ^= PIN_LED;
}

/* ---- 按键消抖状态机 ---- */
typedef enum { KEY_IDLE, KEY_PRESS, KEY_RELEASE } key_state_t;

int main(void)
{
    config_led_output();
    printf("配置后 CRL = 0x%08X（PA5 对应的 4 位被写成 0x3）\n", (unsigned)gpioa.CRL);

    /* 模拟的按键电平序列：1=松开，0=按下。
       第 2~3 个采样是按下时的抖动（0,1,0），最后松手时也有抖动 */
    const uint8_t samples[] = {1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1};
    const size_t n = sizeof(samples) / sizeof(samples[0]);

    key_state_t state = KEY_IDLE;
    uint8_t led_state = 0;
    int press_count = 0;

    printf("\n采样序列（1=松开 0=按下）：");
    for (size_t i = 0; i < n; ++i) printf("%u", samples[i]);
    printf("\n\n逐次处理：\n");

    for (size_t i = 0; i < n; ++i) {
        gpioa.IDR = samples[i] ? PIN_KEY : 0u;   /* 把采样值放进输入寄存器 */
        uint8_t level = key_level();

        switch (state) {
        case KEY_IDLE:
            if (level == 0) {
                state = KEY_PRESS;
                printf("  [%2zu] 电平 0 -> 进入待确认（可能是抖动）\n", i);
            }
            break;
        case KEY_PRESS:
            if (level == 0) {
                /* 连续两次采样都是按下，确认为真实按下 */
                press_count++;
                led_toggle();
                led_state = (gpioa.ODR & PIN_LED) ? 1 : 0;
                printf("  [%2zu] 电平 0 -> 确认按下：LED %s（第 %d 次）\n",
                       i, led_state ? "亮" : "灭", press_count);
                state = KEY_RELEASE;
            } else {
                printf("  [%2zu] 电平 1 -> 是抖动，回到空闲\n", i);
                state = KEY_IDLE;
            }
            break;
        case KEY_RELEASE:
            if (level == 1) {
                printf("  [%2zu] 电平 1 -> 已松手，回到空闲\n", i);
                state = KEY_IDLE;
            }
            break;
        }
    }

    printf("\n结果：采样中出现了 %d 次电平跳变，但只确认了 %d 次真实按下\n", 6, press_count);
    printf("      抖动被状态机滤掉——这就是 K04 的 switch 与 K03 的位操作合起来的用处\n");

    return 0;
}
