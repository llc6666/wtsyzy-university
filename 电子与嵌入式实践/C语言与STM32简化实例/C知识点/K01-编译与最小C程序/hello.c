/* K01 最小 C 程序：用于观察预处理 -> 编译 -> 汇编 -> 链接四个阶段。
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic hello.c -o hello.exe
 */

#include <stdio.h>

#define LED_PIN 5          /* 宏：预处理阶段就被替换掉 */
#define GREET "hello"

int add(int a, int b);     /* 声明：告诉编译器函数的样子 */

int main(void)
{
    int sum = add(2, 3);
    printf("%s: sum=%d pin=%d\n", GREET, sum, LED_PIN);
    return 0;
}

int add(int a, int b)
{
    return a + b;
}
