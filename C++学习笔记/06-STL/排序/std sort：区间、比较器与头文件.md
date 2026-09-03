---
title: std sort：区间、比较器与头文件
tags:
  - c-plus-plus
  - stl
  - sorting
created: 2026-09-01
---

# std::sort：区间、比较器与头文件

## 头文件

```cpp
#include <algorithm>   // std::sort
#include <functional>  // std::greater
#include <vector>      // std::vector
```

## 语法

```cpp
std::sort(first, last);
std::sort(first, last, compare);
```

排序区间是左闭右开：`[first, last)`，`last` 不参与排序。

## 示例

```cpp
#include <algorithm>

int a[5] = {4, 1, 5, 2, 3};
std::sort(a, a + 5);  // 升序
```

降序：

```cpp
#include <algorithm>
#include <functional>

std::sort(a, a + 5, std::greater<int>());
```

只排序下标 `1` 到 `3`：

```cpp
std::sort(a + 1, a + 4);
```

比较函数：

```cpp
bool cmp(int x, int y) {
    return x > y;
}

std::sort(a, a + 5, cmp);
```

`cmp(x, y)` 返回 `true`，表示 `x` 应排在 `y` 前面。不要使用 `<=`。

## vector

```cpp
#include <algorithm>
#include <vector>

std::vector<int> a;
a.push_back(4);
a.push_back(1);
a.push_back(5);
std::sort(a.begin(), a.end());
```

## 版本边界

`std::sort` 在旧标准中就存在。C++11 的 lambda 需要较新的语言标准；旧版编译器使用普通比较函数。

## 关联

排序总览：算法选择与复杂度 · 二维数组的 sort 排序
