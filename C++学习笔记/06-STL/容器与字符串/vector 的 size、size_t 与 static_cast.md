---
title: vector 的 size、size_t 与 static_cast
tags:
  - c-plus-plus
  - stl
  - vector
  - type-conversion
created: 2026-08-27
---

# vector 的 size、size_t 与 static_cast

## 一句话结论

`vector::size()`返回当前元素数量，返回类型是容器的无符号大小类型，通常写成 `std::vector<T>::size_type`，实际使用中常见为 `std::size_t`。

`static_cast<int>(v.size())`只是要求编译器进行显式类型转换，不会检查转换后的 `int`是否装得下原来的大小。

## 它解决什么问题

你在遍历 `vector`时会写：

```cpp
for (std::size_t i = 0; i < values.size(); ++i) {
}
```

此时下标变量和 `size()`返回类型一致。若使用 `int`，编译器可能提示有符号整数与无符号整数比较。

## 工作机制

- `size()`表示当前元素数量，不表示容量，也不表示最后一个下标。
- 长度为 `n`的 `vector`合法下标是 `0`到 `n - 1`。
- `size()`的复杂度是常数级。
- `size_type`是容器为表示大小定义的类型，标准并不要求它必须叫 `std::size_t`，但标准库容器通常以无符号类型实现它。
- `static_cast<T>(value)`明确要求把表达式转换为 `T`，转换可能发生窄化或数值截断。

## 最小示例

```cpp
#include <cstddef>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> values{10, 20, 30};

    std::vector<int>::size_type n = values.size();
    std::cout << n << '\n'; // 3

    for (std::size_t i = 0; i < values.size(); ++i) {
        std::cout << values[i] << ' ';
    }
    std::cout << '\n';

    int smallSize = static_cast<int>(values.size());
    std::cout << smallSize << '\n'; // 3
}
```

## `static_cast`到底做了什么

```cpp
std::size_t n = 3;
int x = static_cast<int>(n);
```

可以把它读成：

> 把表达式 `n`转换成 `int`，并把转换结果交给 `x`。

它提高了意图的可读性，也能避免一部分隐式转换警告。它不等于“安全转换函数”，不会自动替你验证范围。

## 推荐写法

### 只做下标遍历

```cpp
for (std::size_t i = 0; i < values.size(); ++i) {
    std::cout << values[i] << '\n';
}
```

### 算法需要整数且能证明范围足够小

```cpp
int n = static_cast<int>(values.size());
```

使用前必须有前提：`values.size()`的值确实不会超出 `int`范围。

### 不要为了消除警告盲目强转

```cpp
int n = static_cast<int>(values.size());
```

如果容器可能很大，这句会隐藏范围问题。应继续使用 `size_type`或`std::size_t`，或者先做范围检查。

## 常见错误与边界

### 把 `size()`当成最后一个下标

```cpp
std::vector<int> v{10, 20, 30};
// v[v.size()] = 40; // 错误：下标 3 越界
v[v.size() - 1] = 40; // 最后一个元素，下标 2
```

空容器时不能直接使用 `v.size() - 1`作为下标，因为无符号类型会发生下溢。

### 有符号与无符号比较

```cpp
for (int i = 0; i < values.size(); ++i) {
    // 小规模代码通常能工作，但可能产生比较警告
}
```

更稳妥的遍历方式是让 `i`使用 `std::size_t`，或使用范围 `for`：

```cpp
for (int value : values) {
    std::cout << value << '\n';
}
```

## 使用规则

1. `size()`是元素数量；最后一个下标是 `size() - 1`，且仅在非空时成立。
2. 下标循环优先使用 `std::size_t`或容器的 `size_type`。
3. `static_cast`只负责显式转换，不负责范围验证。
4. 强转前先确认目标类型能够表示原值。
5. 不要用强转掩盖空容器、越界或有符号性设计问题。

## 关联

vector 初始化：大小、初值与列表初始化 · 数组：基本定义与语法 · 个人错误集：C++ 实战踩坑记录

## 来源

- C++ 工作草案，[vector]：`size_type`、`size()`和容量接口。<https://eel.is/c++draft/vector>
- C++ 工作草案，[static_cast]：`static_cast<T>(v)`的显式转换语义。<https://eel.is/c++draft/expr.static.cast>
