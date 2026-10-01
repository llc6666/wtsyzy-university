/* K03 读码验证：复现 CrashCourse《单身狗》示例里的位测试写法问题。
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic singledog-check.c -o singledog-check.exe
 *
 * 原写法：(rst & (1u << leftPos)) == 1
 * 修正写：(rst & (1u << leftPos)) != 0
 * 本程序用两组数据对比两者，观察原写法在什么情况下碰巧正确、什么情况下失效。
 */

#include <stdio.h>

/* 原写法（逐字保留示例中的判断方式） */
static void find_two_original(const int arr[], int size, int *d1, int *d2)
{
    int rst = arr[0];
    int leftPos = 0;
    for (int i = 1; i < size; i++) rst = rst ^ arr[i];
    for (leftPos = 0; leftPos < 32; leftPos++) {
        if ((rst & (1u << leftPos)) == 1u) break; /* 问题在“== 1” */
    }
    *d1 = 0;
    *d2 = 0;
    if (leftPos >= 32) {
        printf("  [原写法] 未找到等于 1 的位（避免继续移位造成未定义行为）\n");
        return;
    }
    for (int i = 0; i < size; i++) {
        if ((arr[i] & (1u << leftPos)) == 1u) *d1 = *d1 ^ arr[i]; /* 同一问题 */
        else *d2 = *d2 ^ arr[i];
    }
    printf("  [原写法] leftPos=%2d -> %d, %d\n", leftPos, *d1, *d2);
}

/* 修正写法 */
static void find_two_fixed(const int arr[], int size, int *d1, int *d2)
{
    int rst = arr[0];
    int leftPos = 0;
    for (int i = 1; i < size; i++) rst = rst ^ arr[i];
    for (leftPos = 0; leftPos < 32; leftPos++) {
        if ((rst & (1u << leftPos)) != 0) break;  /* 修正：无符号 + 判非零 */
    }
    *d1 = 0;
    *d2 = 0;
    for (int i = 0; i < size; i++) {
        if ((arr[i] & (1u << leftPos)) != 0) *d1 = *d1 ^ arr[i];
        else *d2 = *d2 ^ arr[i];
    }
    printf("  [修正后] leftPos=%2d -> %d, %d\n", leftPos, *d1, *d2);
}

static void run(const char *label, const int arr[], int size, int expect1, int expect2)
{
    int a1, a2, b1, b2;
    printf("%s  期望：%d 和 %d\n", label, expect1, expect2);
    find_two_original(arr, size, &a1, &a2);
    find_two_fixed(arr, size, &b1, &b2);
}

int main(void)
{
    /* A 组：rst = 3 ^ 4 = 7，最低位为 1，原写法碰巧命中 bit0 */
    int arrA[] = {1, 1, 2, 2, 3, 4};
    run("A 组 {1,1,2,2,3,4}", arrA, 6, 3, 4);

    /* B 组：rst = 3 ^ 5 = 6，最低位为 0，原写法找不到“等于 1”的位 */
    int arrB[] = {1, 1, 2, 2, 3, 5};
    run("B 组 {1,1,2,2,3,5}", arrB, 6, 3, 5);

    /* C 组：rst = 6 ^ 10 = 12，同样最低位为 0 */
    int arrC[] = {1, 1, 2, 2, 6, 10};
    run("C 组 {1,1,2,2,6,10}", arrC, 6, 6, 10);

    return 0;
}
