/* K04 控制流与循环
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic control-flow-demo.c -o control-flow-demo.exe
 *
 * 目的：练三件事——分支要覆盖全部情况、多个条件可能同时成立、
 * 以及嵌入式里主循环长什么样。
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* 闰年：能被 4 整除，但逢 100 不闰、逢 400 又闰 */
static bool is_leap(int year)
{
    if (year % 4 != 0) return false;      /* 第一层：4 */
    if (year % 100 != 0) return true;
    return (year % 400 == 0);             /* 第二层：100 与 400 */
}

/* 雨滴：三个条件不互斥，不能用 else if 串起来 */
static void raindrops(int n, char out[], size_t size)
{
    out[0] = '\0';                        /* 调用方给的缓冲，先清空 */
    if (n % 3 == 0) strncat(out, "Pling", size - strlen(out) - 1);
    if (n % 5 == 0) strncat(out, "Plang", size - strlen(out) - 1);
    if (n % 7 == 0) strncat(out, "Plong", size - strlen(out) - 1);
    if (out[0] == '\0') snprintf(out, size, "%d", n);
}

int main(void)
{
    /* 1. 闰年三层分支，逐条验证 */
    int years[] = {1996, 1900, 2000, 2023, 2024};
    for (size_t i = 0; i < sizeof(years) / sizeof(years[0]); ++i) {
        printf("1) %d 是闰年吗：%s\n", years[i], is_leap(years[i]) ? "是" : "不是");
    }

    /* 2. 雨滴：30 同时命中两条，用来证明不能用 else if */
    char buf[32];
    int nums[] = {28, 30, 34, 105};
    for (size_t i = 0; i < sizeof(nums) / sizeof(nums[0]); ++i) {
        raindrops(nums[i], buf, sizeof(buf));
        printf("2) %d -> %s\n", nums[i], buf);
    }

    /* 3. 嵌入式主循环的雏形：一直跑，靠状态决定做什么 */
    printf("3) 主循环演示（跑 5 轮后退出，真实 MCU 上是 while(1)）\n");
    int tick = 0;
    int led_on = 0;
    while (tick < 5) {
        /* 每两轮翻转一次，等价于一个最简状态机 */
        if (tick % 2 == 0) {
            led_on = !led_on;
        }
        printf("   第 %d 轮：LED %s\n", tick, led_on ? "亮" : "灭");
        ++tick;
    }

    /* 4. 循环里的边界：无符号倒序最容易写错 */
    printf("4) 无符号倒序（写成 i > 0，最后一次处理索引 0）：");
    for (unsigned i = 3; i > 0; --i) {
        printf("%u ", i - 1);
    }
    printf("\n   若写成 i >= 0，无符号永远成立，会一直减到回绕\n");

    return 0;
}
