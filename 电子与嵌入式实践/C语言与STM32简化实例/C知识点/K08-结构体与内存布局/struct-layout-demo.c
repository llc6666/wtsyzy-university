/* K08 结构体、对齐与内存布局
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic struct-layout-demo.c -o struct-layout-demo.exe
 *
 * 目的：看清「结构体成员不是紧挨着排的」，以及成员顺序会影响结构体大小。
 * 这两件事在把结构体映射到硬件寄存器时是硬约束。
 */

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* 成员顺序不同，大小可能不同：编译器会在成员之间插入填充字节 */
struct bad_order {
    uint8_t  a;    /* 1 字节 */
    uint32_t b;    /* 4 字节，前面要补 3 字节对齐 */
    uint8_t  c;    /* 1 字节 */
};                 /* 末尾再补到 4 的倍数 */

struct good_order {
    uint32_t b;    /* 4 */
    uint8_t  a;    /* 1 */
    uint8_t  c;    /* 1 */
};                 /* 补 2 字节 */

/* 模拟一组寄存器：全 32 位成员，没有填充 */
struct regs {
    volatile uint32_t CRL;
    volatile uint32_t CRH;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
};

/* 位域：把几个标志塞进一个字节 */
struct flags {
    unsigned tx_done : 1;
    unsigned rx_done : 1;
    unsigned error   : 1;
    unsigned         : 5;   /* 未使用的 5 位 */
};

/* 按值传参会整体拷贝；按指针传只传地址 */
struct point { int x; int y; };

static void move_by_value(struct point p)
{
    p.x += 10;               /* 改的是副本 */
}

static void move_by_pointer(struct point *p)
{
    p->x += 10;              /* 改的是原对象 */
}

int main(void)
{
    printf("1) bad_order  大小=%zu，成员大小之和=%zu（差的就是填充字节）\n",
           sizeof(struct bad_order),
           sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t));
    printf("   good_order 大小=%zu（把大的放前面更省空间）\n",
           sizeof(struct good_order));

    printf("2) 各成员偏移：\n");
    printf("   bad_order:  a@%zu b@%zu c@%zu\n",
           offsetof(struct bad_order, a),
           offsetof(struct bad_order, b),
           offsetof(struct bad_order, c));
    printf("   good_order: b@%zu a@%zu c@%zu\n",
           offsetof(struct good_order, b),
           offsetof(struct good_order, a),
           offsetof(struct good_order, c));

    printf("3) 全 32 位成员的寄存器组：大小=%zu，偏移 CRL@%zu CRH@%zu IDR@%zu ODR@%zu\n",
           sizeof(struct regs),
           offsetof(struct regs, CRL), offsetof(struct regs, CRH),
           offsetof(struct regs, IDR), offsetof(struct regs, ODR));
    printf("   每个成员都比前一个大 4，没有填充——这正是库能把它映射到寄存器的前提\n");

    struct flags f = {0};
    f.tx_done = 1;
    printf("4) 位域：sizeof=%zu 字节，tx_done=%u rx_done=%u\n",
           sizeof(struct flags), f.tx_done, f.rx_done);
    printf("   位域节省空间，但位序由编译器决定，做硬件寄存器映射时要谨慎\n");

    struct point pt = {1, 2};
    move_by_value(pt);
    printf("5) 按值传参后：x=%d（没变，传的是副本）\n", pt.x);
    move_by_pointer(&pt);
    printf("   按指针传参后：x=%d（变了）\n", pt.x);

    printf("6) 结构体可以直接赋值（数组不行）：");
    struct point q = pt;
    printf("q.x=%d q.y=%d\n", q.x, q.y);

    return 0;
}
