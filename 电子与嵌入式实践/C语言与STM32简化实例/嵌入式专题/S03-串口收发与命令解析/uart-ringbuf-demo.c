/* S03 串口收发：环形缓冲 + 中断写入 + 主循环解析命令
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic uart-ringbuf-demo.c -o uart-ringbuf-demo.exe
 *
 * 目的：把 K05（字符数组）、K10（volatile 标志）、K12（环形缓冲）串成一个真实场景：
 * 中断把字节塞进环形缓冲，主循环攒成一行再解析执行。
 * 本程序实际为单线程顺序模拟；count++/count-- 不是原子操作。
 * 不能将本结构直接移植为中断安全队列，真实并发需另加同步或重构。
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define RB_SIZE 16                 /* 容量取 2 的幂，方便用位运算绕回 */
#define LINE_MAX 32

/* ---- 环形缓冲 ---- */
typedef struct {
    uint8_t buf[RB_SIZE];
    volatile size_t head;          /* 写指针（中断里改） */
    size_t          tail;          /* 读指针（主循环改） */
    volatile size_t count;
} ringbuf_t;

static ringbuf_t g_rx;             /* 接收缓冲：中断写、主循环读 */

static void rb_init(ringbuf_t *rb)
{
    memset(rb->buf, 0, sizeof(rb->buf));
    rb->head = rb->tail = rb->count = 0;
}

static bool rb_write(ringbuf_t *rb, uint8_t b)
{
    if (rb->count == RB_SIZE) return false;      /* 满了就丢，绝不越界 */
    rb->buf[rb->head] = b;
    rb->head = (rb->head + 1) & (RB_SIZE - 1);   /* 容量是 2 的幂，用掩码绕回 */
    rb->count++;
    return true;
}

static bool rb_read(ringbuf_t *rb, uint8_t *out)
{
    if (rb->count == 0) return false;
    *out = rb->buf[rb->tail];
    rb->tail = (rb->tail + 1) & (RB_SIZE - 1);
    rb->count--;
    return true;
}

/* ---- 模拟串口接收中断：收到一个字节就塞进缓冲 ---- */
static int g_dropped = 0;

static void uart_rx_isr(uint8_t byte)
{
    if (!rb_write(&g_rx, byte)) {
        g_dropped++;               /* 缓冲满，统计丢弃（真实工程要据此调容量） */
    }
}

/* ---- 命令解析 ---- */
static void execute(const char *line)
{
    if (strcmp(line, "LED ON") == 0)       printf("  -> 执行：开灯\n");
    else if (strcmp(line, "LED OFF") == 0) printf("  -> 执行：关灯\n");
    else if (strncmp(line, "PWM ", 4) == 0) {
        int duty = 0;
        if (sscanf(line + 4, "%d", &duty) == 1) {
            printf("  -> 执行：PWM 占空比 %d%%\n", duty);
        }
    } else if (line[0] == '\0') {
        /* 空行忽略 */
    } else {
        printf("  -> 未知命令：%s\n", line);
    }
}

int main(void)
{
    rb_init(&g_rx);

    /* 模拟串口收到的字节流：两条命令 + 一段超出缓冲长度的突发数据 */
    const char *stream = "LED ON\nPWM 60\nLED OFF\n0123456789ABCDEFGHIJ";

    printf("模拟收到字节流：%s\n\n", stream);

    char line[LINE_MAX];
    size_t line_len = 0;

    for (size_t i = 0; stream[i] != '\0'; ++i) {
        uart_rx_isr((uint8_t)stream[i]);      /* 每个字节都先进中断 */

        /* 主循环：能读多少读多少，攒到换行就执行一条命令 */
        uint8_t b;
        while (rb_read(&g_rx, &b)) {
            if (b == '\n') {
                line[line_len] = '\0';        /* 补结尾，成为字符串 */
                printf("[主循环] 收到一行：%s\n", line);
                execute(line);
                line_len = 0;                 /* 清空，准备下一行 */
            } else if (line_len + 1 < LINE_MAX) {
                line[line_len++] = (char)b;   /* 留一个字节给 \0 */
            } else {
                printf("[主循环] 行太长，截断\n");
                line_len = 0;
            }
        }
    }

    /* ---- 突发场景：中断连续来 20 字节，主循环暂时来不及读 ---- */
    printf("\n[突发] 模拟主循环正忙时，中断连续收到 20 字节：\n");
    for (int k = 0; k < 20; ++k) {
        uart_rx_isr((uint8_t)('A' + k));
    }
    printf("  缓冲内积压 %zu 字节，已丢弃 %d 个\n", (size_t)g_rx.count, g_dropped);

    int read_out = 0;
    uint8_t tmp;
    while (rb_read(&g_rx, &tmp)) {
        read_out++;
    }
    printf("  主循环腾出手后读出 %d 字节，剩余 %zu 字节\n", read_out, (size_t)g_rx.count);
    printf("  （丢弃的字节不会补回来——所以缓冲容量要按最坏情况的突发量来定）\n");

    printf("\n[统计] 丢弃字节 %d 个（缓冲容量 %d）\n", g_dropped, RB_SIZE);
    printf("[说明] 中断只负责写入，解析与执行都在主循环；缓冲满了宁可丢也不能越界\n");

    return 0;
}
