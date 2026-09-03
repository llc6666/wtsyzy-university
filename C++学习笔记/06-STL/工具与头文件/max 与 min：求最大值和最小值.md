---
title: max 与 min：求最大值和最小值
tags:
  - c-plus-plus
  - stl
  - algorithm
  - comparison
created: 2026-08-28
---

# max 与 min：求最大值和最小值

## 一句话结论

`std::max` 返回较大的值，`std::min` 返回较小的值。使用它们通常需要：

```cpp
#include <algorithm>
```

## 两个值比较

```cpp
#include <algorithm>
#include <iostream>

int main() {
    int a = 10;
    int b = 20;

    std::cout << std::max(a, b) << '\n'; // 20
    std::cout << std::min(a, b) << '\n'; // 10
}
```

基本格式：

```cpp
std::max(值1, 值2)
std::min(值1, 值2)
```

## 多个值比较

C++11 及以后可以传入初始化列表：

```cpp
int mx = std::max({1, 7, 3, 5}); // 7
int mn = std::min({1, 7, 3, 5}); // 1
```

需要包含：

```cpp
#include <algorithm>
#include <initializer_list>
```

有些实现会通过其他头文件间接提供相关声明，但代码不应依赖这种间接包含。

C++11 以前只能嵌套调用：

```cpp
int mx = std::max(std::max(1, 7), std::max(3, 5));
int mn = std::min(std::min(1, 7), std::min(3, 5));
```

## 直接用于表达式

```cpp
int x = 8;
int y = 12;

int larger = std::max(x + 1, y - 3); // max(9, 9)，结果为 9
int smaller = std::min(x * 2, y);    // min(16, 12)，结果为 12
```

## 限制一个值的范围

这是很常见的组合：

```cpp
int value = 120;
int limited = std::min(std::max(value, 0), 100);
```

执行过程：

1. `std::max(value, 0)`：保证结果至少为 `0`。
2. `std::min(..., 100)`：保证结果至多为 `100`。

所以 `limited` 一定在 `[0, 100]` 范围内。

C++17 还可以写成：

```cpp
int limited = std::clamp(value, 0, 100);
```

需要：

```cpp
#include <algorithm>
```

## 类型必须兼容

```cpp
std::max(1, 2);       // 正确
std::max(1.0, 2.0);   // 正确
```

不要随意混合不同类型：

```cpp
// std::max(1, 2.5); // 可能无法推导出同一个模板类型
```

明确转换：

```cpp
std::max(1.0, static_cast<double>(2));
```

## 比较结构体或自定义规则

默认情况下，`max` 和 `min` 使用 `<` 比较。也可以传入比较器：

```cpp
#include <algorithm>
#include <string>

struct Student {
    std::string name;
    int score;
};

Student a{"A", 80};
Student b{"B", 90};

Student best = std::max(a, b, [](const Student& x, const Student& y) {
    return x.score < y.score;
});
```

这里比较器返回 `true` 表示“左边应排在右边之前”，因此分数更高的对象会被视为最大值。

## 与 `if` 的区别

```cpp
int result;

if (a > b) {
    result = a;
} else {
    result = b;
}
```

可以简化为：

```cpp
int result = std::max(a, b);
```

`if` 适合多个分支或需要执行多条操作；`max`、`min` 适合直接取得比较结果。

## 常见错误

### 忘记头文件

```cpp
// 缺少 #include <algorithm>
int x = std::max(1, 2);
```

### 把 `max` 当成输出格式

```cpp
std::cout << std::max(a, b); // 求最大值并输出
```

`max` 只负责返回比较结果，是否输出由 `cout` 决定。

### 多值列表中的类型不一致

```cpp
// std::max({1, 2.5, 3}); // 初始化列表元素类型需要一致
```

## 使用规则

1. 使用 `std::max`、`std::min` 前包含 `<algorithm>`。
2. 两个值直接传入两个参数。
3. C++11 以后多个值可以使用初始化列表。
4. 比较的类型应一致或能明确转换到同一类型。
5. 限制区间可以组合 `min(max(x, low), high)`；C++17 以后可使用 `std::clamp`。
6. 需要复杂分支时使用 `if`，不要强行嵌套 `min`、`max`。

## 关联

C++ 学习笔记 · vector：动态数组的基本用法 · 向下取整：floor 与整数除法

## 来源

- C++ 标准库，[std::max]。<https://en.cppreference.com/w/cpp/algorithm/max>
- C++ 标准库，[std::min]。<https://en.cppreference.com/w/cpp/algorithm/min>
- C++ 标准库，[std::clamp]。<https://en.cppreference.com/w/cpp/algorithm/clamp>
