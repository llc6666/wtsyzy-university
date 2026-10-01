---
title: list-ops_题面
tags:
  - 素材
created: 2026-10-01
status: 外来素材（中文译写）
source: exercism/c（MIT 许可）
---

# 题面：列表操作

> 来源：exercism/c 仓库 `exercises/practice/list-ops`（MIT 许可）。本文为中文译写，英文原件未保留。

实现一组基础的列表操作。

在函数式语言里，`length`、`map`、`reduce` 这类列表操作非常常见。请用**不调用现成函数**的方式实现它们。

要实现的操作如下（名字以头文件为准）：

- `append`：给定两个列表，把第二个列表的所有元素接到第一个列表末尾
- `concatenate`：给定一系列列表，把所有元素合并成一个扁平的列表
- `filter`：给定一个判断函数和列表，返回所有满足该判断的元素组成的列表
- `length`：返回列表中元素的总数
- `map`：给定一个函数和列表，返回把该函数作用在每个元素上的结果列表
- `foldl`：给定函数、列表和初始累积值，从**左**往右把每个元素折叠进累积值
- `foldr`：给定函数、列表和初始累积值，从**右**往左折叠
- `reverse`：返回元素顺序完全颠倒的新列表

注意：传给 fold 系列函数的**参数顺序是有意义的**（`foldl` 与 `foldr` 的差别就在顺序）。

## 官方接口要点

`list_t` 不是链式节点，而是带柔性数组的连续结构：

```c
typedef struct {
   size_t length;
   list_element_t elements[];
} list_t;
```
