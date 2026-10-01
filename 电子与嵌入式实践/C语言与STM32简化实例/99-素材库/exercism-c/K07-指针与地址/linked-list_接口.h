/* 链表练习的官方接口（注释已译为中文，标识符保留原文）
 * 来源：exercism/c 仓库 exercises/practice/linked-list/linked_list.h（MIT 许可）
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

typedef int ll_data_t;
struct list;

// 创建一个新（空）链表
struct list *list_create(void);

// 统计链表中的元素个数
size_t list_count(const struct list *list);

// 在链表尾部插入元素
void list_push(struct list *list, ll_data_t item_data);

// 从链表尾部移除元素并返回它
ll_data_t list_pop(struct list *list);

// 在链表头部插入元素
void list_unshift(struct list *list, ll_data_t item_data);

// 从链表头部移除元素并返回它
ll_data_t list_shift(struct list *list);

// 删除持有指定数据的那个节点
void list_delete(struct list *list, ll_data_t data);

// 销毁整个链表
// 调用之后，list 会变成一个悬空指针
void list_destroy(struct list *list);

#endif
