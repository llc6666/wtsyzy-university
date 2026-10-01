/* 列表操作练习的官方接口（注释已译为中文，标识符保留原文）
 * 来源：exercism/c 仓库 exercises/practice/list-ops/list_ops.h（MIT 许可）
 */

#ifndef LIST_OPS_H
#define LIST_OPS_H

#include <stdlib.h>
#include <stdbool.h>

typedef int list_element_t;

/* 注意：list_t 不是链式节点，而是「长度 + 柔性数组」的连续结构 */
typedef struct {
   size_t length;
   list_element_t elements[];
} list_t;

// 构造一个新列表
list_t *new_list(size_t length, list_element_t elements[]);

// 把第二个列表的元素追加到第一个列表末尾，返回新列表
list_t *append_list(list_t *list1, list_t *list2);

// 过滤：返回所有满足 filter 条件的元素组成的新列表
list_t *filter_list(list_t *list, bool (*filter)(list_element_t));

// 返回列表长度
size_t length_list(list_t *list);

// 映射：返回把 map 作用在每个元素上的结果列表
list_t *map_list(list_t *list, list_element_t (*map)(list_element_t));

// 从左往右折叠（累积）
list_element_t foldl_list(list_t *list, list_element_t initial,
                          list_element_t (*foldl)(list_element_t,
                                                  list_element_t));

// 从右往左折叠（累积）
list_element_t foldr_list(list_t *list, list_element_t initial,
                          list_element_t (*foldr)(list_element_t,
                                                  list_element_t));

// 反转：返回元素顺序颠倒的新列表
list_t *reverse_list(list_t *list);

// 销毁整个列表
// 调用之后，list 会变成一个悬空指针
void delete_list(list_t *list);

#endif
