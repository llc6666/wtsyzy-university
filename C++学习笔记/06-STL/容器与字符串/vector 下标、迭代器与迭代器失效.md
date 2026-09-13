---
title: vector 下标、迭代器与迭代器失效
tags:
  - c-plus-plus
  - stl
  - vector
  - debugging
status: public
---

# vector 下标、迭代器与迭代器失效

## 一句话结论

`vector` 有两套常用访问方式：下标接收整数位置，迭代器通过解引用取得元素。先判断手里的对象是什么，再选择访问方式。`end()` 是尾后位置，不能解引用；改变容器结构后，旧迭代器也可能失效。

## 1. 先分清三个对象

设有：

```cpp
std::vector<int> values{10, 20, 30};
std::size_t i = 1;
auto it = values.begin();
```

| 对象 | 实际含义 | 正确用法 |
| --- | --- | --- |
| `i` | 整数下标 | `values[i]` |
| `it` | 指向某个元素的迭代器 | `*it`、`it[1]`、`*(it + 1)` |
| `values.end()` | 最后一个元素之后的位置 | 只能比较、递减后使用，不能直接 `*values.end()` |

错误的典型写法是：

```cpp
values[values.begin()];
```

`operator[]` 要的是整数位置，`values.begin()` 返回的是迭代器。两者属于不同访问模型，不能混用。

## 2. 下标访问与迭代器访问

### 下标：已经知道位置时使用

```cpp
for (std::size_t i = 0; i < values.size(); ++i) {
    std::cout << values[i] << '\n';
}
```

`values[i]` 不检查越界。需要边界检查时可以使用 `values.at(i)`，越界会抛出 `std::out_of_range`。

### 迭代器：需要遍历或配合算法时使用

```cpp
for (auto it = values.begin(); it != values.end(); ++it) {
    std::cout << *it << '\n';
}
```

`vector` 的迭代器支持随机访问，因此以下写法都成立：

```cpp
auto first = values.begin();
std::cout << first[1] << '\n';
std::cout << *(first + 2) << '\n';
```

如果代码只要求“向后走几步”，而不依赖随机访问容器，可以使用 `std::next(it, step)`。这能让意图更清楚，也能迁移到只支持前向迭代的容器。

## 3. `begin`、`cbegin`、`end` 与 `cend`

- `begin()` 返回可修改元素的迭代器。
- `cbegin()` 返回只读迭代器，不能通过它修改元素。
- `end()` 和 `cend()` 都表示尾后位置。
- 尾后位置没有元素，`*values.end()` 是未定义行为。

反向遍历时，先判断容器是否为空，再从 `end()` 递减：

```cpp
if (!values.empty()) {
    for (auto it = values.end(); it != values.begin();) {
        --it;
        std::cout << *it << '\n';
    }
}
```

空 `vector` 的 `begin()` 与 `end()` 可以比较，但不能递减 `begin()`，也不能解引用任意一个位置。

## 4. 迭代器什么时候会失效

迭代器失效指迭代器仍然保存着旧地址或旧位置，但它已经不能安全代表原来的元素。最常见的检查规则如下：

| 操作 | 需要警惕的情况 |
| --- | --- |
| `push_back`、`emplace_back` | 发生重新分配时，所有旧迭代器、引用和指针失效；即使没有重新分配，旧的 `end()` 也不能继续当作新的尾后位置 |
| `insert` | 发生重新分配时全部失效；没有重新分配时，插入位置及其后的迭代器失效 |
| `erase` | 被删除位置及其后的迭代器失效，函数返回新的后继迭代器 |
| `reserve` | 触发重新分配时，已有迭代器全部失效 |
| `clear` | 指向被清空元素的迭代器全部失效 |

安全模式是：结构修改后重新获取迭代器；循环删除时使用 `erase` 的返回值。

```cpp
for (auto it = values.begin(); it != values.end();) {
    if (*it < 20) {
        it = values.erase(it);
    } else {
        ++it;
    }
}
```

`reserve` 只能减少重新分配的概率，不能让所有修改都保持旧迭代器有效。代码仍需按照具体操作的失效规则判断。

## 5. 最小可编译验证

```cpp
#include <iostream>
#include <iterator>
#include <vector>

int main() {
    std::vector<int> values{10, 20, 30};
    const std::size_t index = 1;
    auto it = values.begin();
    auto cit = values.cbegin();

    std::cout << values[index] << '\n';       // 20
    std::cout << *it << '\n';                 // 10
    std::cout << it[1] << '\n';               // 20
    std::cout << *(it + 2) << '\n';           // 30
    std::cout << cit[2] << '\n';              // 30，只读
    std::cout << std::distance(values.cbegin(), values.cend()) << '\n'; // 3
}
```

预期输出：

```text
20
10
20
30
30
3
```

编译命令：

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -g vector_iterator_demo.cpp -o vector_iterator_demo
```

## 6. 写代码前的检查清单

1. 手里的对象是整数下标、迭代器，还是元素本身？
2. 访问位置是否可能等于 `size()`？等于时它已经是尾后位置。
3. 容器是否为空？空容器不能解引用。
4. 最近是否执行了 `insert`、`erase`、`push_back`、`reserve` 或 `clear`？执行后重新检查旧迭代器。
5. 如果只需要读，是否可以使用 `cbegin()`、`cend()` 或范围 `for`，减少误修改？

## 关联

- [vector 的 size、size_t 与 static_cast](vector%20的%20size、size_t%20与%20static_cast.md)
- [vector 初始化：大小、初值与列表初始化](vector%20初始化：大小、初值与列表初始化.md)
- [Microsoft Learn：vector class](https://learn.microsoft.com/en-us/cpp/standard-library/vector-class?view=msvc-170)
- [C++ Working Draft N4950](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/n4950.pdf)

## 发布边界

本文只保留 C++ 标准容器的通用规则、最小示例和验证命令。个人学习记录、私有路径、课程安排、题单台账和外部原文均未纳入。
