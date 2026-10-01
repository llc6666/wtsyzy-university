/* K03 位运算与掩码：用普通变量模拟一个 32 位 GPIO 寄存器。
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic gpio-mask-demo.c -o gpio-mask-demo.exe
 *
 * 目的：在没有开发板的情况下，先把「置位 / 清零 / 翻转 / 测试 / 改字段」
 * 这五种寄存器操作的位运算练熟。真板子上只是把变量名换成寄存器地址。
 */

#include <stdio.h>
#include <stdint.h>

/* 模拟 GPIOA 的输出数据寄存器（真实硬件上它是个固定地址的 volatile 变量） */
static volatile uint32_t gpioa_odr = 0x00000000u;

/* 用移位构造掩码：第 n 位为 1，其余为 0。
 * 注意写 1u 而不是 1：1 是 int，左移 31 位会溢出，属于未定义行为。 */
#define PIN(n) (1u << (n))

/* 打印 32 位二进制，从高位到低位 */
static void print_bits(uint32_t v)
{
    for (int i = 31; i >= 0; --i) {
        putchar((v >> i) & 1u ? '1' : '0');
        if (i % 8 == 0 && i != 0) putchar(' ');
    }
}

int main(void)
{
    printf("起始      "); print_bits(gpioa_odr); printf("  (0x%08X)\n", gpioa_odr);

    /* 1. 置位：把第 5 位置 1，其他位不动 */
    gpioa_odr |= PIN(5);
    printf("置位 PA5  "); print_bits(gpioa_odr); printf("  (0x%08X)\n", gpioa_odr);

    /* 2. 再置位第 0 位，验证前一次的结果没有被破坏 */
    gpioa_odr |= PIN(0);
    printf("置位 PA0  "); print_bits(gpioa_odr); printf("  (0x%08X)\n", gpioa_odr);

    /* 3. 清零：把第 5 位清 0，其他位不动。~PIN(5) 得到“只有第 5 位是 0”的掩码 */
    gpioa_odr &= ~PIN(5);
    printf("清零 PA5  "); print_bits(gpioa_odr); printf("  (0x%08X)\n", gpioa_odr);

    /* 4. 翻转：异或，相同为 0 不同为 1，第 0 位取反 */
    gpioa_odr ^= PIN(0);
    printf("翻转 PA0  "); print_bits(gpioa_odr); printf("  (0x%08X)\n", gpioa_odr);

    /* 5. 测试某一位：结果是 0 或非 0，不是 0 或 1（除非右移出来） */
    printf("测试 PA0：raw=%u, 归一化为 0/1: %u\n",
           (unsigned)(gpioa_odr & PIN(0)),
           (unsigned)((gpioa_odr & PIN(0)) != 0));

    /* 6. 改一个多位字段：把低 4 位整体改成 0xA，其余位不动。
     *    这是 STM32 里配置“引脚模式”的通用写法：先清字段，再写新值。 */
    gpioa_odr = (gpioa_odr & ~0x0Fu) | (0xAu & 0x0Fu);
    printf("低4位改A "); print_bits(gpioa_odr); printf("  (0x%08X)\n", gpioa_odr);

    /* 7. 读出一个多位字段：右移到最低位，再用掩码挡住多余的位 */
    uint32_t crl_like = 0x00000048u;           /* 模拟配置寄存器：引脚1的模式在 bit[7:4] */
    unsigned pin = 1;
    uint32_t mode = (crl_like >> (4u * pin)) & 0x0Fu;
    printf("读字段   pin%u 的模式字段 = 0x%X\n", pin, mode);

    /* 8. 位带惯用法的好处：多次设置同一位，结果与设置一次相同（幂等） */
    gpioa_odr |= PIN(3);
    gpioa_odr |= PIN(3);
    gpioa_odr |= PIN(3);
    printf("重复置位 "); print_bits(gpioa_odr); printf("  (0x%08X)\n", gpioa_odr);

    return 0;
}
