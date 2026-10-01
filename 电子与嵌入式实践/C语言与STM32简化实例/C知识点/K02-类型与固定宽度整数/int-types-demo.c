/* K02 类型、位宽与固定宽度整数
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic int-types-demo.c -o int-types-demo.exe
 *
 * 目的：把「类型宽度不同会导致什么后果」用能跑的例子记住，
 * 尤其是溢出、回绕、格式符和有无符号比较这四个坑。
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>

int main(void)
{
    /* 1. 各类型在当前机器上的实际宽度（嵌入式上 int 可能是 16 位，不能假定） */
    printf("1) 宽度：char=%zu short=%zu int=%zu long=%zu long long=%zu 指针=%zu\n",
           sizeof(char), sizeof(short), sizeof(int), sizeof(long),
           sizeof(long long), sizeof(void *));
    printf("   int 范围：%d .. %d\n", INT_MIN, INT_MAX);
    printf("   uint32_t 范围：0 .. %" PRIu32 "\n", UINT32_MAX);

    /* 2. 棋盘麦粒：第 64 格是 2^63，总和是 2^64-1，都装不进 32 位 */
    uint64_t on_square_64 = (uint64_t)1 << 63;          /* 必须用 1u 的 64 位版本 */
    uint64_t total = (uint64_t)0 - 1;                   /* 全部位为 1，即 2^64-1 */
    printf("2) 第 64 格 = %" PRIu64 "\n", on_square_64);
    printf("   总和     = %" PRIu64 "\n", total);

    /* 3. 同样的计算用 32 位做，会静默回绕（编译不报错，结果全错） */
    uint32_t small = (uint32_t)1 << 31;
    printf("3) 用 uint32_t 表示第 32 格 = %" PRIu32 "\n", small);
    uint32_t wrapped = small + small;                   /* 应该进位到 2^32，回绕成 0 */
    printf("   再加一次（回绕后）      = %" PRIu32 "\n", wrapped);

    /* 4. 无符号减法的回绕：0 - 1 变成最大值，循环条件因此失效 */
    uint32_t zero = 0;
    printf("4) (uint32_t)0 - 1 = %" PRIu32 "（不是 -1）\n", zero - 1);

    /* 5. 有符号与无符号比较：结果按无符号规则算，容易反直觉 */
    int signed_neg = -1;
    unsigned int unsigned_one = 1u;
    printf("5) -1 < 1 ? %s\n", (signed_neg < (int)unsigned_one) ? "真" : "假");
    if (signed_neg < (int)unsigned_one) {
        printf("   先转成同一类型比较才安全\n");
    }
    /* 直接比较会发生隐式转换，下面这行在严格警告下会提示 */
    printf("   直接比较 -1 < 1u 的结果是：%s（因为 -1 被转成了很大的无符号数）\n",
           ((unsigned)signed_neg < unsigned_one) ? "真" : "假");

    /* 6. 格式符必须匹配类型：uint64_t 用 PRIu64，不要猜 %lld 或 %lu */
    uint64_t v = 1234567890123ull;
    printf("6) 用 PRIu64 打印：%" PRIu64 "；用 PRIx64 打印十六进制：0x%" PRIx64 "\n", v, v);

    return 0;
}
