/* S01 单片机程序骨架
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic mcu-skeleton-demo.c -o mcu-skeleton-demo.exe
 *
 * 目的：在没有开发板的情况下，把 MCU 程序的四个阶段跑一遍——
 * 初始化（相当于启动文件干的事）、主循环、中断置标志、主循环处理标志。
 *
 * 模拟方式：
 *   - 用全局变量模拟「硬件状态」
 *   - 用普通函数模拟「中断服务函数 ISR」，由主循环按节奏调用它
 *   - 用 volatile 标志模拟「中断与主循环之间唯一的连接方式」
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* ---- 模拟硬件状态 ---- */
static volatile uint8_t  g_tick_flag = 0;    /* 定时器中断置起：必须为 volatile */
static volatile uint8_t  g_key_flag  = 0;    /* 外部中断置起 */
static volatile uint32_t g_tick_count = 0;

static uint32_t g_led_toggles = 0;           /* 主循环统计：LED 翻转了几次 */

/* ---- 模拟中断服务函数：只做最少的事 ---- */
static void timer_isr(void)                  /* 相当于 TIMx_IRQHandler */
{
    g_tick_flag = 1;                         /* 只置标志 */
    g_tick_count++;
}

static void key_isr(void)                    /* 相当于 EXTIx_IRQHandler */
{
    g_key_flag = 1;
}

/* ---- 模拟启动阶段：真实启动文件做的是 .data 拷贝、.bss 清零、设置栈顶 ---- */
static void startup_like_init(void)
{
    /* 这里没有真实的 .data/.bss，用一句显式初始化代替：
       全局变量若未显式初始化，C 保证它们为零——但局部变量不会 */
    g_tick_flag = 0;
    g_key_flag = 0;
    g_tick_count = 0;
    g_led_toggles = 0;
    printf("[启动] 时钟、外设、全局状态初始化完成\n");
}

/* ---- 主循环里的业务处理 ---- */
static void handle_tick(void)
{
    g_led_toggles++;
    printf("[主循环] 定时器事件 %u：翻转 LED（第 %u 次）\n",
           (unsigned)g_tick_count, (unsigned)g_led_toggles);
}

static void handle_key(void)
{
    printf("[主循环] 按键事件：切换工作模式\n");
}

int main(void)
{
    startup_like_init();

    printf("[主循环] 进入 while(1)\n");

    /* 用有限轮次模拟永远运行的主循环 */
    for (int loop = 0; loop < 12; ++loop) {
        /* ---- 模拟硬件在这段时间里触发了中断 ---- */
        if (loop % 3 == 2) {
            timer_isr();                     /* 每 3 轮来一次定时器中断 */
        }
        if (loop == 5 || loop == 9) {
            key_isr();                       /* 第 5、9 轮来一次按键中断 */
        }

        /* ---- 主循环：查标志、处理、清标志 ---- */
        if (g_tick_flag) {
            g_tick_flag = 0;                 /* 先清，再处理 */
            handle_tick();
        }
        if (g_key_flag) {
            g_key_flag = 0;
            handle_key();
        }

        /* 没有事件时主循环空转（真实工程里可以做低功耗休眠） */
    }

    printf("\n[统计] 定时器中断 %u 次，LED 翻转 %u 次\n",
           (unsigned)g_tick_count, (unsigned)g_led_toggles);
    printf("[说明] 中断里只置标志，处理放在主循环——这是裸机程序最基本的分工\n");

    return 0;
}
