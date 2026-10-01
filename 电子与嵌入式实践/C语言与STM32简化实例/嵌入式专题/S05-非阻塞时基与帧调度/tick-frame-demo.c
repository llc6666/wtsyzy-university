/* S05 非阻塞时基与帧调度：SysTick 毫秒计数、状态机、回绕
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic tick-frame-demo.c -o tick-frame-demo.exe
 *
 * 目的：把「阻塞延时为什么不行」「非阻塞到点做事怎么写」「uint32 回绕怎么判」跑通。
 * 模拟方式：g_ms 当 SysTick 累加的毫秒计数；阻塞版在 delay 里空转推时间，
 *           非阻塞版逐毫秒轮询两个「下次动作时间」。红绿灯状态机 + 并行心跳任务对照。
 * 原型：本地 OLED 工程原型（SysTick_Config + OLED_WaitUntil）。
 */

#include <stdio.h>
#include <stdint.h>

/* ---- 模拟 SysTick：真实硬件里 SysTick_Handler 每 1ms 把它 +1 ---- */
static volatile uint32_t g_ms = 0;

static void sys_tick_handler(void)
{
    g_ms++;
}

/* ---- 红绿灯状态机 ---- */
typedef enum { LIGHT_RED = 0, LIGHT_GREEN, LIGHT_YELLOW } light_t;

static const char *light_name[] = { "红灯", "绿灯", "黄灯" };
static const uint32_t light_duration[3] = { 3000, 2000, 500 };   /* 各亮多久 */

static void switch_light(light_t st)
{
    printf("[t=%5ums] %s亮\n", (unsigned)g_ms, light_name[st]);
}

/* 阻塞式等待：原型是江协 Delay_ms（SysTick 轮询空转）。
 * 时钟照走（sys_tick_handler 继续加），但 CPU 在这个循环里出不去：
 * 心跳任务排在它后面，一次都轮不到。 */
static void delay_ms_blocking(uint32_t ms)
{
    uint32_t start = g_ms;
    while (g_ms - start < ms) {
        sys_tick_handler();       /* 空转：硬件时钟照走，CPU 什么都不做 */
    }
}

/* 非阻塞判断：还没到返回 1。用有符号差值，回绕也正确（原型 OLED_WaitUntil） */
static int not_yet(uint32_t now, uint32_t target)
{
    return (int32_t)(now - target) < 0;
}

int main(void)
{
    uint32_t t;

    printf("== 1. 阻塞式：红灯 3s -> 绿灯 2s -> 黄灯 0.5s；心跳本应 1000ms 一次 ==\n");
    g_ms = 0;
    {
        light_t st = LIGHT_RED;

        switch_light(st);                 /* t=0 红灯亮 */
        delay_ms_blocking(light_duration[st]);
        st = LIGHT_GREEN;
        switch_light(st);                 /* Delay 完才轮得到下一行 */
        delay_ms_blocking(light_duration[st]);
        st = LIGHT_YELLOW;
        switch_light(st);
        delay_ms_blocking(light_duration[st]);
        printf("[t=%5ums] 一轮结束。心跳任务排在 Delay 后面，6000ms 里 0 次（本应 6 次）\n",
               (unsigned)g_ms);
    }

    printf("\n== 2. 非阻塞：同一个时基，两件事同轮进行 ==\n");
    g_ms = 0;
    {
        light_t st = LIGHT_RED;
        uint32_t switch_at = 0;           /* 下次切灯时间 */
        uint32_t beat_at = 1000;          /* 下次心跳时间 */
        int beat_count = 0;

        for (t = 0; t <= 6000; t++) {
            if (!not_yet(t, beat_at)) {   /* 到点了，跳一次心跳 */
                beat_count++;
                printf("[t=%5ums] 心跳 #%d\n", (unsigned)t, beat_count);
                beat_at += 1000;
            }
            if (!not_yet(t, switch_at)) { /* 到点了，切灯 */
                switch_light(st);
                switch_at += light_duration[st];
                st = (st == LIGHT_YELLOW) ? LIGHT_RED : (light_t)(st + 1);
            }
            sys_tick_handler();           /* 本轮动作做完，时间推进 1ms */
        }
        printf("6000ms：心跳 %d 次一次不落，灯按时切换，两件事都没耽误\n",
               beat_count);
    }

    printf("\n== 3. 回绕：uint32_t 计满归零 ==\n");
    {
        uint32_t start = 0xFFFFFFF0u;     /* 离计满只差 16 */
        uint32_t target = start + 20;     /* 目标落在回绕之后 */
        uint32_t now = start;
        int i;

        printf("起始 now=0xFFFFFFF0，等 20ms，目标 target=0x%08X\n",
               (unsigned)target);
        for (i = 0; i <= 20; i += 5) {
            printf("  now=0x%08X  有符号差值 %3d  -> %s\n",
                   (unsigned)now,
                   (int)((int32_t)(now - target)),
                   not_yet(now, target) ? "还没到" : "到点");
            now += 5;
        }
        printf("错误写法 now >= target：now=0xFFFFFFF0 >= 0x00000004 为真，\n");
        printf("  -> 误判\"已到点\"，提前 20ms 触发；有符号差值法则等到回绕后才放行\n");
    }

    return 0;
}
