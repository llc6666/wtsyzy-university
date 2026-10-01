/* K06 函数、参数传递与数组传参
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic func-args-demo.c -o func-args-demo.exe
 *
 * 目的：把「值传递改不了外面、数组会退化成指针、长度必须另传」这三件事跑通。
 */

#include <stdio.h>
#include <stddef.h>

/* 1. 值传递：改的是副本，外面不变 */
static void try_swap_by_value(int a, int b)
{
    int t = a; a = b; b = t;
}

/* 2. 指针传递：按地址改，外面跟着变 */
static void swap_by_pointer(int *a, int *b)
{
    int t = *a; *a = *b; *b = t;
}

/* 3. 数组参数：写成 int arr[] 或 int *arr 是同一回事，长度信息已经丢了 */
static int array_sum(const int *arr, size_t len)
{
    int s = 0;
    for (size_t i = 0; i < len; ++i) s += arr[i];
    return s;
}

/* 4. 需要返回多个结果时，用指针参数带出去 */
static void min_max(const int *arr, size_t len, int *out_min, int *out_max)
{
    *out_min = arr[0];
    *out_max = arr[0];
    for (size_t i = 1; i < len; ++i) {
        if (arr[i] < *out_min) *out_min = arr[i];
        if (arr[i] > *out_max) *out_max = arr[i];
    }
}

/* 5. 原地逆序：只要首地址和长度就够 */
static void reverse_array(int *arr, size_t len)
{
    for (size_t i = 0; i < len / 2; ++i) {
        int t = arr[i];
        arr[i] = arr[len - 1 - i];
        arr[len - 1 - i] = t;
    }
}

int main(void)
{
    int x = 1, y = 2;
    try_swap_by_value(x, y);
    printf("1) 值传递后：x=%d y=%d（没换）\n", x, y);
    swap_by_pointer(&x, &y);
    printf("   指针传递后：x=%d y=%d（换了）\n", x, y);

    int data[] = {4, 7, 1, 9, 3};
    size_t len = sizeof(data) / sizeof(data[0]);
    printf("2) 在本函数里 sizeof(data)=%zu（整个数组）\n", sizeof(data));
    printf("   传给函数后，函数里只能拿到指针，所以必须另传 len=%zu\n", len);
    printf("3) 求和 = %d\n", array_sum(data, len));

    int lo, hi;
    min_max(data, len, &lo, &hi);
    printf("4) 最小值=%d 最大值=%d（通过指针参数带回来）\n", lo, hi);

    reverse_array(data, len);
    printf("5) 原地逆序后：");
    for (size_t i = 0; i < len; ++i) printf("%d ", data[i]);
    printf("\n   （原数组被改了，因为传进去的是地址）\n");

    /* 6. const 的位置：两种含义不同 */
    int v = 10;
    const int *p1 = &v;        /* 不能通过 p1 改 v */
    int *const p2 = &v;        /* p2 不能再指向别处，但能改 v */
    *p2 = 20;
    printf("6) const int *p（数据只读）与 int *const p（指针只读）不是一回事；v=%d\n", v);
    printf("   通过 p1 读到的值 = %d\n", *p1);

    return 0;
}
