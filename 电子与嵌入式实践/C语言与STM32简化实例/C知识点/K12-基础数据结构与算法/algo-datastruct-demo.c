/* K12 基础数据结构与算法
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic algo-datastruct-demo.c -o algo-datastruct-demo.exe
 *
 * 目的：练三样嵌入式天天要用的东西——二分查找、环形缓冲、以及排序的取舍。
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

/* ============ 1. 二分查找：返回下标，找不到返回 -1 ============ */
static int binary_search_index(int value, const int *arr, size_t len)
{
    if (arr == NULL || len == 0) return -1;

    size_t low = 0;
    size_t high = len;              /* 用「前闭后开」区间 [low, high) */
    while (low < high) {
        size_t mid = low + (high - low) / 2;   /* 这样写不会溢出 */
        if (arr[mid] < value) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }
    if (low < len && arr[low] == value) return (int)low;
    return -1;
}

/* ============ 2. 环形缓冲：串口收发最常用的结构 ============ */
#define RB_SIZE 8                    /* 容量做成 2 的幂，方便用位运算绕回 */

typedef struct {
    uint8_t  buf[RB_SIZE];
    size_t   head;                   /* 写指针 */
    size_t   tail;                   /* 读指针 */
    size_t   count;                  /* 当前元素个数：用它区分满与空 */
} ringbuf_t;

static void rb_init(ringbuf_t *rb)
{
    memset(rb, 0, sizeof(*rb));
}

static bool rb_write(ringbuf_t *rb, uint8_t byte)
{
    if (rb->count == RB_SIZE) return false;    /* 满了，拒绝写入 */
    rb->buf[rb->head] = byte;
    rb->head = (rb->head + 1) % RB_SIZE;       /* 绕回 */
    rb->count++;
    return true;
}

static bool rb_read(ringbuf_t *rb, uint8_t *out)
{
    if (rb->count == 0) return false;          /* 空了 */
    *out = rb->buf[rb->tail];
    rb->tail = (rb->tail + 1) % RB_SIZE;
    rb->count--;
    return true;
}

/* ============ 3. 排序：qsort 的比较函数 ============ */
static int cmp_int(const void *a, const void *b)
{
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);        /* 不用减法，避免溢出 */
}

int main(void)
{
    /* 1. 二分查找 */
    int sorted[] = {4, 8, 12, 16, 23, 28, 32};
    size_t n = sizeof(sorted) / sizeof(sorted[0]);
    int targets[] = {23, 4, 32, 5};
    for (size_t i = 0; i < sizeof(targets) / sizeof(targets[0]); ++i) {
        int idx = binary_search_index(targets[i], sorted, n);
        if (idx >= 0) printf("1) 找 %2d：下标 %d\n", targets[i], idx);
        else          printf("1) 找 %2d：不在数组中\n", targets[i]);
    }

    /* 2. 环形缓冲：写满、读出、再写 */
    ringbuf_t rb;
    rb_init(&rb);
    printf("2) 写入 0..9（容量只有 %d）：", RB_SIZE);
    for (uint8_t i = 0; i < 10; ++i) {
        if (!rb_write(&rb, i)) printf("[满]");
    }
    printf("\n   当前元素个数 = %zu\n", rb.count);

    uint8_t v;
    printf("   读出全部：");
    while (rb_read(&rb, &v)) printf("%d ", v);
    printf("\n   读完后个数 = %zu，再读一次返回 %s\n",
           rb.count, rb_read(&rb, &v) ? "成功" : "失败");

    /* 3. qsort */
    int to_sort[] = {5, 2, 9, 1, 7};
    size_t m = sizeof(to_sort) / sizeof(to_sort[0]);
    qsort(to_sort, m, sizeof(to_sort[0]), cmp_int);
    printf("3) qsort 结果：");
    for (size_t i = 0; i < m; ++i) printf("%d ", to_sort[i]);
    printf("\n");

    /* 4. 二分查找的前提：必须先有序 */
    printf("4) 二分查找的前提是数组有序；对无序数组要先排序（排序 O(n log n)，查找 O(log n)）\n");

    return 0;
}
