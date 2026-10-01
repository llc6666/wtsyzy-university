/* K09 动态内存与所有权
 * 编译：gcc -std=c17 -Wall -Wextra -Wpedantic malloc-ownership-demo.c -o malloc-ownership-demo.exe
 *
 * 目的：练「谁申请、谁使用、谁释放」这条链，并看清忘记释放与释放后使用分别是什么后果。
 * 注意：本程序只演示正确写法，不故意制造崩溃（重复释放的后果是未定义的）。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    int data;
    struct node *next;
} node_t;

/* 创建一个节点：本函数申请，调用方（或链表销毁函数）释放 */
static node_t *node_create(int data)
{
    node_t *n = malloc(sizeof(*n));      /* 用 sizeof(*n) 而不是 sizeof(node_t)，改类型时不会漏改 */
    if (n == NULL) {
        return NULL;                     /* 申请失败必须检查，不能直接用 */
    }
    n->data = data;
    n->next = NULL;
    return n;
}

/* 头插法 */
static void list_push(node_t **head, int data)
{
    node_t *n = node_create(data);
    if (n == NULL) return;
    n->next = *head;
    *head = n;                           /* 通过二级指针改调用方的头指针 */
}

/* 销毁：本函数负责释放全部节点 */
static void list_destroy(node_t **head)
{
    node_t *cur = *head;
    while (cur != NULL) {
        node_t *next = cur->next;        /* 先存下一个，再释放当前 */
        free(cur);
        cur = next;
    }
    *head = NULL;                        /* 让调用方的指针失效，避免悬空 */
}

static void list_print(const node_t *head)
{
    for (const node_t *p = head; p != NULL; p = p->next) {
        printf("%d ", p->data);
    }
    printf("\n");
}

int main(void)
{
    /* 1. malloc 与 calloc 的差别：calloc 会把内存清零 */
    int *a = malloc(5 * sizeof(int));
    int *b = calloc(5, sizeof(int));
    if (a == NULL || b == NULL) {
        printf("1) 内存申请失败\n");
        free(a); free(b);
        return 1;
    }
    printf("1) malloc 未初始化，内容不确定；calloc 清零：");
    for (int i = 0; i < 5; ++i) printf("%d ", b[i]);
    printf("\n");

    /* 2. 用完释放，并把指针置空 */
    free(a);
    a = NULL;                            /* 置空后再误用会立刻暴露，而不是静默出错 */
    free(b);
    b = NULL;
    printf("2) 已释放并把指针置 NULL；此时再 free 一次仍是错的，但判空可以防住误用\n");
    if (a == NULL) {
        printf("   a == NULL，不会被误用\n");
    }

    /* 3. 一条完整的链表生命周期 */
    node_t *head = NULL;
    list_push(&head, 3);
    list_push(&head, 1);
    list_push(&head, 4);
    printf("3) 链表内容：");
    list_print(head);

    list_destroy(&head);
    printf("   销毁后 head = %s\n", head == NULL ? "NULL" : "非 NULL");
    printf("   （销毁函数把调用方的指针也置空了，这就是所有权交接的写法）\n");

    /* 4. 静态分配：裸机里更常用的方式 */
    static int pool[16];                 /* 编译期就定好，不存在申请失败 */
    memset(pool, 0, sizeof(pool));
    pool[0] = 42;
    printf("4) 静态/固定数组：大小=%zu，编译期确定，不会失败也不会碎片\n", sizeof(pool));
    printf("   pool[0]=%d\n", pool[0]);

    printf("5) 小结：PC 上可以随意 malloc；裸机上更倾向编译期定死的数组与内存池\n");

    return 0;
}
