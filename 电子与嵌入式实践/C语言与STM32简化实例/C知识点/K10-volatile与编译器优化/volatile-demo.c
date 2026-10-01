/* K10 volatile 与编译器优化
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic -O2 -pthread volatile-demo.c -o volatile-demo.exe
 *
 * 目的：用另一个线程扮演「中断服务函数」，演示跨线程通知应使用原子变量。
 * volatile 的编译器可见性差异由 volatile-asm-probe.c 单独展示。
 *
 * 重要说明：volatile 不是线程同步原语，也不能消除数据竞争。
 * 跨线程或 RTOS 任务间通知要使用原子操作、锁或临界区；硬件寄存器和
 * 中断共享标志的 volatile 用法仍需结合具体平台的同步规则。
 */

#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_SPIN 300           /* 每轮约 1 毫秒，最多等 300 毫秒 */

static atomic_bool flag_atomic;

/* 扮演中断服务函数：延时后置起标志 */
static void *isr_like_thread(void *arg)
{
    (void)arg;
    usleep(50 * 1000);           /* 等 50 毫秒，模拟中断稍后发生 */
    atomic_store_explicit(&flag_atomic, true, memory_order_release);
    return NULL;
}

/* 等待原子标志：获得线程间同步关系 */
static long wait_atomic(void)
{
    long spins = 0;
    while (!atomic_load_explicit(&flag_atomic, memory_order_acquire)) {
        usleep(1000);                       /* 模拟主循环每轮干的活 */
        if (++spins >= MAX_SPIN) break;
    }
    return spins;
}

int main(void)
{
    pthread_t th;
    atomic_init(&flag_atomic, false);
    printf("启动扮演中断的线程（50 毫秒后置起标志）\n");

    int rc = pthread_create(&th, NULL, isr_like_thread, NULL);
    if (rc != 0) {
        printf("线程创建失败，返回 %d\n", rc);
        return 1;
    }
    long spins = wait_atomic();
    printf("1) 原子版本：循环 %ld 次后退出，flag=%d\n", spins,
           atomic_load_explicit(&flag_atomic, memory_order_acquire) ? 1 : 0);
    pthread_join(th, NULL);

    printf("2) 说明：\n");
    printf("   - 跨线程通知使用原子读写，避免普通变量数据竞争\n");
    printf("   - volatile 只约束编译器访问，不提供线程同步；差异见汇编探针\n");
    printf("   - 硬件寄存器和中断标志是否需要额外屏障，要按芯片与并发模型核对\n");

    return 0;
}
