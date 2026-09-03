---
title: 向下取整：floor 与整数除法
tags:
  - c-plus-plus
  - arithmetic
  - rounding
created: 2026-08-28
---

# 向下取整：floor 与整数除法

## 一句话结论

向下取整是取“不大于原数的最大整数”。浮点数使用 `std::floor`；非负整数相除时，普通 `/` 就能得到向下取整，但含负数时 C++ 整数除法是向 0 截断，不能直接当作 floor。

## 浮点数：使用 `std::floor`

```cpp
#include <cmath>

std::floor(3.8);   // 3.0
std::floor(-3.8);  // -4.0
```

`std::floor` 返回不大于参数的最大整数值，返回类型仍是浮点类型。

如果需要保存为整数：

```cpp
long long result = static_cast<long long>(std::floor(x));
```

前提是结果能被目标整数类型表示。

## 非负整数除法：直接使用 `/`

```cpp
int a = 7;
int b = 2;

int result = a / b; // 3
```

因为 `7 / 2 = 3.5`，非负整数除法去掉小数部分后正好是向下取整。

常见算竞写法：

```cpp
int groups = n / k;
```

当 `n >= 0` 且 `k > 0` 时，表示完整的 `k` 个一组的数量。

## 负数时要小心

C++ 整数除法向 0 截断：

```cpp
-7 / 2 // -3
```

但数学上的向下取整是：

```text
floor(-3.5) = -4
```

因此：

```cpp
-7 / 2
```

不是严格意义上的向下取整。整数除法的截断规则见 C++ 算术运算规则。

## 有负数时实现整数向下取整

```cpp
long long floor_div(long long a, long long b) {
    // b 必须不为 0
    long long q = a / b;
    long long r = a % b;

    if (r != 0 && ((r > 0) != (b > 0))) {
        --q;
    }

    return q;
}
```

示例：

```cpp
floor_div(7, 2);    // 3
floor_div(-7, 2);   // -4
floor_div(7, -2);   // -4
floor_div(-7, -2);  // 3
```

判断逻辑：C++ 先得到向 0 截断的商；如果有余数且商的方向与真实结果不一致，就把商再减 1。

## 不要把强制转换当成向下取整

```cpp
double x = -3.8;
int a = static_cast<int>(x); // -3，向 0 截断
```

如果需要数学上的向下取整：

```cpp
int b = static_cast<int>(std::floor(x)); // -4
```

正数情况下两者结果可能相同，负数情况下会不同。

## 使用规则

1. 浮点数向下取整：`std::floor(x)`，需要 `<cmath>`。
2. 非负整数相除：`a / b` 通常就是向下取整，要求 `b > 0`。
3. 含负数的整数除法：`/` 是向 0 截断，不一定是向下取整。
4. 不要用 `static_cast<int>` 代替负数的 `floor`。
5. 除数不能为 `0`。

## 关联

C++ 学习笔记 · 整数类型范围与选择 · 百分比输出与小数位格式化

## 来源

- C++ 标准库，[std::floor]：返回不大于参数的最大整数值。<https://en.cppreference.com/w/cpp/numeric/math/floor>
- C++ 语言，[Arithmetic operators]：整数除法向零截断。<https://en.cppreference.com/w/cpp/language/operator_arithmetic>
