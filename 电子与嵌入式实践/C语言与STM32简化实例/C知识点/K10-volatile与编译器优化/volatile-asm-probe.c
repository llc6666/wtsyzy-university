/* K10 汇编探针：不在循环体里放任何函数调用，让编译器有优化空间。
 * 生成汇编：gcc -std=c17 -O2 -S volatile-asm-probe.c -o volatile-asm-probe.s
 *
 * 对比 wait_plain 与 wait_volatile 生成的汇编，
 * 就能确定地看到 volatile 到底改变了什么，而不依赖运行时现象。
 */

volatile int flag_volatile = 0;
int          flag_plain    = 0;

/* 没有 volatile：编译器可以把读操作提到循环外 */
void wait_plain(void)
{
    while (flag_plain == 0) {
        /* 空循环 */
    }
}

/* 有 volatile：每次循环都必须重新从内存读 */
void wait_volatile(void)
{
    while (flag_volatile == 0) {
        /* 空循环 */
    }
}
