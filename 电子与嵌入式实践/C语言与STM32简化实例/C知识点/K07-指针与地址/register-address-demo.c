/* K07 指针与地址：理解「地址是什么」「结构体指针怎么映射成一排寄存器」。
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic register-address-demo.c -o register-address-demo.exe
 *
 * 真实 STM32 里 GPIOA 的寄存器是一块固定地址的内存，库把它包成结构体指针。
 * 这里用一个真实的静态变量充当那块内存，地址由系统分配，行为与硬件一致。
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

/* 模拟 STM32 的一组 GPIO 寄存器（真实头文件里就是这个顺序） */
typedef struct {
    volatile uint32_t CRL;   /* 端口配置低寄存器 */
    volatile uint32_t CRH;   /* 端口配置高寄存器 */
    volatile uint32_t IDR;   /* 输入数据寄存器 */
    volatile uint32_t ODR;   /* 输出数据寄存器 */
} GPIO_TypeDef;

static GPIO_TypeDef gpioa_sim;                 /* 当作 GPIOA 的寄存器块 */
#define GPIOA_SIM ((GPIO_TypeDef *)&gpioa_sim) /* 基地址转成结构体指针 */

int main(void)
{
    /* 1. 指针里存的是地址，解引用是「按地址去取那块内存」 */
    int x = 42;
    int *p = &x;
    printf("1) x=%d  &x=%p  p=%p  *p=%d\n", x, (void *)&x, (void *)p, *p);

    *p = 100;                /* 通过指针改的是同一块内存 */
    printf("   改后 x=%d\n", x);

    /* 2. 指针加 1 前进的字节数 == 指向类型的大小，不是 1 字节 */
    int arr[3] = {10, 20, 30};
    int *q = arr;
    printf("2) q=%p  q+1=%p  相差 %td 字节（sizeof(int)=%zu）\n",
           (void *)q, (void *)(q + 1),
           (char *)(q + 1) - (char *)q, sizeof(int));

    uint8_t *b = (uint8_t *)arr;   /* 同一块内存，换一种看法 */
    printf("   同一个地址当 uint8_t* 看：+1 前进 %td 字节\n",
           (char *)(b + 1) - (char *)b);

    /* 3. 数组名在多数场合就是首元素地址，arr[i] 等价于 *(arr+i) */
    printf("3) arr[2]=%d  *(arr+2)=%d  2[arr]=%d（下标写法只是语法糖）\n",
           arr[2], *(arr + 2), 2[arr]);

    /* 4. 结构体的每个成员相对基地址有一个偏移，寄存器就是靠这个偏移排开的 */
    printf("4) 基址=%p\n", (void *)GPIOA_SIM);
    printf("   CRL 偏移 %2zu 字节 -> %p\n", offsetof(GPIO_TypeDef, CRL),
           (void *)&GPIOA_SIM->CRL);
    printf("   CRH 偏移 %2zu 字节 -> %p\n", offsetof(GPIO_TypeDef, CRH),
           (void *)&GPIOA_SIM->CRH);
    printf("   IDR 偏移 %2zu 字节 -> %p\n", offsetof(GPIO_TypeDef, IDR),
           (void *)&GPIOA_SIM->IDR);
    printf("   ODR 偏移 %2zu 字节 -> %p\n", offsetof(GPIO_TypeDef, ODR),
           (void *)&GPIOA_SIM->ODR);

    /* 5. 用结构体指针操作寄存器，写法与 SPL 完全一致 */
    GPIOA_SIM->ODR = 0;
    GPIOA_SIM->ODR |= (1u << 5);          /* PA5 输出高 */
    GPIOA_SIM->CRL = (GPIOA_SIM->CRL & ~(0x0Fu << (4u * 5))) | (0x03u << (4u * 5));
    printf("5) ODR=0x%08X  CRL=0x%08X\n", GPIOA_SIM->ODR, GPIOA_SIM->CRL);

    /* 6. 不用结构体，直接拿地址算也一样，只是可读性差 */
    volatile uint32_t *odr_raw = &gpioa_sim.ODR;
    *odr_raw &= ~(1u << 5);
    printf("6) 直接按地址操作后 ODR=0x%08X（与 5 的结果一致说明两条路等价）\n",
           GPIOA_SIM->ODR);

    /* 7. 一个必须记住的边界：函数返回后，局部变量的地址就失效了 */
    printf("7) 不要返回局部变量地址，也不要把已 free 的内存地址留着用\n");

    return 0;
}
