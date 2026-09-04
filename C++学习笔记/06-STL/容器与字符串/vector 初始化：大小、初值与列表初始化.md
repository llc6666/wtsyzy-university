---
title: vector 初始化：大小、初值与列表初始化
tags:
  - c-plus-plus
  - stl
  - vector
  - initialization
created: 2026-08-27
---

# vector 初始化：大小、初值与列表初始化

## 一句话结论

`std::vector<int> v(3, 10)`表示“创建 3 个 `int` 元素，并把每个元素初始化为 10”；它不表示容量为 3，也不表示只存入两个数 3 和 10。

圆括号和花括号的含义不同：圆括号通常选择“数量、初值”构造，花括号通常表示元素列表。

## 它解决什么问题

普通数组的长度通常需要在定义时确定。`vector`把“元素类型”和“当前元素数量”封装在一个对象中，适合元素数量会变化、又需要连续下标访问的场景。

## 工作机制

`vector<T>`中的 `T`是元素类型。构造时，括号里的参数会参与构造函数重载选择：

| 写法 | 初始元素 | `size()` |
| --- | --- | ---: |
| `std::vector<int> a;` | 没有元素 | 0 |
| `std::vector<int> b(3);` | 3 个值初始化的 `int`，通常为 0 | 3 |
| `std::vector<int> c(3, 10);` | 3 个 10 | 3 |
| `std::vector<int> d{3, 10};` | 两个元素：3、10 | 2 |
| `std::vector<int> e = {3, 10};` | 两个元素：3、10 | 2 |

对 `int`来说，`vector<int>(3)`中的元素会被值初始化为 0。对自定义类型来说，结果取决于该类型是否支持相应的默认构造。

## 最小示例

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> a;
    std::vector<int> b(3);
    std::vector<int> c(3, 10);
    std::vector<int> d{3, 10};

    std::cout << a.size() << '\n'; // 0
    std::cout << b[0] << ' ' << b.size() << '\n'; // 0 3
    std::cout << c[0] << ' ' << c.size() << '\n'; // 10 3
    std::cout << d[0] << ' ' << d[1] << ' ' << d.size() << '\n'; // 3 10 2
}
```

如果元素数量已经确定，直接用 `vector<int> v(n)`；如果数量会逐步增加，使用空 `vector`配合 `push_back`。

```cpp
std::vector<int> values;
values.push_back(10);
values.push_back(20);
```

## 常见错误与边界

### 把圆括号和花括号混为一谈

```cpp
std::vector<int> a(3, 10); // [10, 10, 10]
std::vector<int> b{3, 10}; // [3, 10]
```

二者都能编译，但表达的初始数据完全不同。

### 把 `size` 和 `capacity` 混为一谈

`size`是当前实际元素数。`capacity`是无需重新分配就能容纳的元素数。`vector<int> v(3, 10)`首先表达的是创建 3 个元素，不能据此推断具体 `capacity()`。

### 用下标写入尚不存在的元素

```cpp
std::vector<int> v;
// v[0] = 10; // 错误：v 仍然没有元素
v.push_back(10); // 先增加元素，再通过下标访问
v[0] = 20;
```

如果想一次创建可写的 3 个元素，应写 `std::vector<int> v(3)`。

## 如何验证

分别创建 `std::vector<int> a(3, 7)` 和 `std::vector<int> b{3, 7}`，输出元素与 `size()`。前者应为三个 7，后者应为元素 3、7；这能直接验证圆括号“大小与初值”和花括号“元素列表”的区别。

## 使用规则

1. `v(n)`优先理解为“创建 n 个元素”。
2. `v(n, value)`理解为“创建 n 个 value 的副本”。
3. `{a, b, c}`理解为“元素列表”。
4. 下标只能访问 `[0, size())`范围内已经存在的元素。
5. 在循环中不断追加元素时使用 `push_back`；已知最终数量时可直接指定大小。

## 关联

vector 的 size、size_t 与 static_cast · 数组：基本定义与语法 · 基础算法训练清单（未收入本公开仓库）

## 来源

- C++ 工作草案，[vector]：容器概览、构造函数和容量语义。<https://eel.is/c++draft/vector>
- 本地参考资料：Haotian-BiJi STL 原始资料
