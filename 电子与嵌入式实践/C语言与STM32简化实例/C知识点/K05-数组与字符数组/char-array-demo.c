/* K05 数组、字符数组与字符串
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic char-array-demo.c -o char-array-demo.exe
 *
 * 目的：把「字符数组 / 结尾的 \0 / 容量 / 单词边界」这四个概念跑一遍，
 * 并演示一个能编译但结果不对的经典错误：复制后没有补 \0。
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* 原地反转：双指针，一个从前往后，一个从后往前 */
static void reverse_in_place(char s[])
{
    size_t len = strlen(s);
    for (size_t i = 0; i < len / 2; ++i) {
        char tmp = s[i];
        s[i] = s[len - 1 - i];
        s[len - 1 - i] = tmp;
    }
}

/* 首字母缩写：靠「前一个字符是不是分隔符」判断单词开头 */
static size_t abbreviate(const char *phrase, char out[], size_t out_size)
{
    size_t n = 0;
    char prev = ' ';                 /* 让第一个字母也算单词开头 */
    for (size_t i = 0; phrase[i] != '\0' && n + 1 < out_size; ++i) {
        char c = phrase[i];
        if (isalpha((unsigned char)c) && (prev == ' ' || prev == '-' || prev == '_')) {
            out[n++] = (char)toupper((unsigned char)c);
        }
        prev = c;
    }
    out[n] = '\0';
    return n;
}

int main(void)
{
    /* 1. 字符数组与 \0：声明的容量要比可见字符多一个字节 */
    char word[] = "hello";
    printf("1) \"hello\" 占 %zu 字节（5 个字母 + 1 个 \\0）\n", sizeof(word));

    /* 2. 原地反转 */
    char s[] = "stressed";
    reverse_in_place(s);
    printf("2) stressed 反转后 = %s\n", s);

    /* 3. 首字母缩写 */
    char acr[32];
    abbreviate("Liquid-crystal display", acr, sizeof(acr));
    printf("3) \"Liquid-crystal display\" -> %s\n", acr);
    abbreviate("Thank George It's Friday!", acr, sizeof(acr));
    printf("   \"Thank George It's Friday!\" -> %s\n", acr);

    /* 4. 经典错误：固定长度复制满容量时不会补 \0，打印会越界 */
    char dst[5];
    memcpy(dst, "ABCDE", sizeof(dst));    /* 正好 5 字节，没有空间放 \0 */
    printf("4) 复制了 5 个字节却没放 \\0，直接打印会读到后面的内存\n");
    printf("   正确做法：复制 n-1 个字节，再手动补 \\0\n");
    char ok[5];
    strncpy(ok, "ABCDE", sizeof(ok) - 1);
    ok[sizeof(ok) - 1] = '\0';
    printf("   修正后：%s（长度 %zu，安全）\n", ok, strlen(ok));

    /* 5. 数组传参后长度信息丢失，sizeof 在函数里是指针大小 */
    printf("5) 在本函数里 sizeof(ok) = %zu；传给函数后它退化成指针\n", sizeof(ok));

    return 0;
}
