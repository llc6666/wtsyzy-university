---
title: sizeof、size() 与 sort 比较器：刷题速查与分界
status: public
updated: 2026-11-12
tags:
  - c-plus-plus
  - stl
  - sizeof
  - sorting
evidence_status: concept_note
---

# sizeof、size() 与 sort 比较器：刷题速查与分界

## 一句话分界

| 问题 | 工具 | 关注点 |
| --- | --- | --- |
| 对象占多少字节 | sizeof | 类型、数组、对齐和表达式是否求值 |
| 容器有几个元素 | size() | 当前元素数量和 size_t |
| 谁排在前面 | 比较器 | 区间、严格排序和容器方向 |

三件事解决不同问题，不能互相替代。

## 1. sizeof：对象大小，不是元素数量

~~~cpp
int a[5];
std::size_t n = sizeof(a) / sizeof(a[0]);   // 仅在 a 仍是数组的作用域内成立
~~~

需要记住：

- sizeof 的结果是字节数，结果类型是 std::size_t。
- 对表达式使用 sizeof 时，表达式通常处于不求值语境；不要把它当成调用函数或执行副作用的工具。
- 数组作为函数参数时会退化为指针，函数内部的 sizeof(a) 得到的是指针大小，不能用来求原数组长度。
- 结构体或类的大小可能包含对齐填充，不能简单把成员大小相加。
- sizeof(vector) 是 vector 对象自身的实现相关大小，不是动态数组的元素数。

数组长度只在“数组本体仍然可见”的位置用 sizeof(a) / sizeof(a[0])。跨函数传递时把长度作为接口的一部分，或者使用带 size() 的容器。

## 2. size()：容器当前有多少元素

常见容器都提供 size()：

| 类型 | 例子 |
| --- | --- |
| vector、array、deque、string | c.size() |
| stack、queue、priority_queue | c.size() |
| set、map、无序容器 | c.size() |

size() 返回无符号的 std::size_t。以下两种循环都可以，关键是避免有符号和无符号混比：

~~~cpp
for (std::size_t i = 0; i < v.size(); ++i) {
    // ...
}

for (int i = 0; i < static_cast<int>(v.size()); ++i) {
    // 只有确认容器大小能放进 int 时才这样写
}
~~~

空容器时不要直接写 v.size() - 1。无符号 0 减 1 会回绕，随后可能形成超大下标或错误循环。需要倒序遍历时，先明确空状态，再选择反向迭代器或有符号索引。

## 3. memset 的字节语义

memset 按字节写入：

~~~cpp
std::memset(a, 0, sizeof a);    // 每个字节写 0
std::memset(a, -1, sizeof a);   // 每个字节写 0xFF
std::memset(a, 1, sizeof a);    // 每个字节写 0x01，不等于把每个 int 设为 1
~~~

给整型数组填充任意数值时，优先使用 std::fill。memset 适合清零、填充字节模式，不能按元素理解第三个参数。

## 4. std::sort：区间是左闭右开

~~~cpp
#include <algorithm>
#include <functional>
#include <vector>

std::sort(v.begin(), v.end());
std::sort(v.begin(), v.begin() + i);                 // 前 i 个元素
std::sort(v.begin(), v.end(), std::greater<int>());   // 降序
~~~

[first, last) 的右端点不参与排序。要处理前 i 个元素，右端点是 begin() + i，不能写成 begin() + i - 1。

自定义比较器的语义是“第一个参数应排在第二个参数前面”：

~~~cpp
bool cmp(const Node& a, const Node& b) {
    if (a.score != b.score) return a.score > b.score;
    return a.id < b.id;
}
~~~

比较器应保持严格弱序。不要写 a <= b，也不要把“是否相等”当成“是否应该排在前面”。多关键字排序按第一关键字分流，相等时再比较下一关键字。

## 5. 四种容器里的比较器位置

| 容器或算法 | 比较器位置 | 方向提示 |
| --- | --- | --- |
| std::sort | 调用时的第三个参数 | > 常用于降序 |
| set、map、multiset | 类型模板参数 | 默认 less 是升序 |
| priority_queue | 第三个模板参数 | greater 形成小根堆，且中间要写底层容器 |
| unordered_set、unordered_map | 没有排序比较器 | 需要顺序时复制到序列容器再排序 |

小根堆的声明：

~~~cpp
std::priority_queue<int, std::vector<int>, std::greater<int>> pq;
~~~

不要把 sort 中的 greater 表示降序机械套到 priority_queue。堆顶规则是比较器决定的优先级，使用前先写出“哪个元素应该在堆顶”。

## 6. 错误对照表

| 写法 | 发生的问题 | 修复动作 |
| --- | --- | --- |
| 函数参数里用 sizeof 求数组长度 | 参数已退化为指针 | 额外传长度或使用容器 |
| 用 sizeof(vector) 求元素数 | 得到对象大小 | 使用 size() |
| 空 vector 直接 size() - 1 | 无符号回绕 | 先判空，再选倒序写法 |
| memset(a, 1, sizeof a) 想把 int 全设为 1 | 按字节写成 0x01010101 | 使用 std::fill |
| sort 的右端点写成 i - 1 | 少处理一个元素 | 使用半开区间 [first, first + i) |
| 比较器返回 <= | 破坏严格排序要求 | 使用 < 或 > 并处理多关键字 |
| priority_queue<int, greater<int>> | 漏写底层容器参数 | 写完整的三个模板参数 |

## 7. 读者自查卡

拿到一段代码时按顺序回答：

1. 这里要问的是字节数还是元素数？
2. 数组本体还在当前作用域吗？
3. size() 的返回类型会不会与索引类型混比？
4. 容器为空时，表达式是否会回绕或访问无效位置？
5. sort 的区间右端点是否是尾后位置？
6. 比较器表达的是“谁在前”，还是误写成“是否相等”？
7. priority_queue 的堆顶是否与需求一致？

## 相关资料

- [sizeof 运算符](https://en.cppreference.com/w/cpp/language/sizeof)
- [容器 size](https://en.cppreference.com/w/cpp/container/vector/size)
- [std::sort](https://en.cppreference.com/w/cpp/algorithm/sort)
- [std::priority_queue](https://en.cppreference.com/w/cpp/container/priority_queue)

## 来源与边界

- 来源：个人 C++ STL 学习笔记，综合数组长度、容器 size、排序区间和比较器专题。
- 本页删除题目编号、训练台账、私有路径和个人测试记录，只保留可迁移的规则、示例和自查问题。
- 本轮只整理，未编译、运行或执行示例。
