---
title: 二维数组的 sort 排序
tags:
  - c-plus-plus
  - array
  - sorting
created: 2026-09-01
---

# 二维数组的 sort 排序

## 一句话结论

二维数组使用 `sort` 前要先明确目标：逐行排序、逐列排序、全部元素展平排序，或按行整体排序。

## 头文件

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
```

## 每行分别排序

```cpp
#include <algorithm>
#include <iostream>

int main() {
    int a[2][4] = {{4, 1, 3, 2}, {8, 6, 7, 5}};

    for (int i = 0; i < 2; ++i) {
        std::sort(a[i], a[i] + 4);
    }
}
```

`a[i]` 表示第 `i` 行，行内元素连续，因此可以直接排序。

## 每列分别排序

同一列元素在内存中通常不连续，不能直接把列首尾地址交给 `std::sort`。应先复制到一维容器，排序后写回：

```cpp
#include <algorithm>
#include <vector>

const int rows = 3;
const int cols = 2;
int a[rows][cols] = {{3, 9}, {1, 7}, {2, 8}};

for (int col = 0; col < cols; ++col) {
    std::vector<int> column;

    for (int row = 0; row < rows; ++row) {
        column.push_back(a[row][col]);
    }

    std::sort(column.begin(), column.end());

    for (int row = 0; row < rows; ++row) {
        a[row][col] = column[row];
    }
}
```

## 全部元素整体排序

真正的内置二维数组按行连续存储，可以展平成一维区间：

```cpp
#include <algorithm>

int a[2][3] = {{4, 1, 6}, {2, 5, 3}};
std::sort(&a[0][0], &a[0][0] + 2 * 3);
```

这种写法会改变全部元素的线性顺序，行的原有含义可能改变。

## 按行整体排序

若要把每一行当作一个整体排序，可以提供比较器：

```cpp
#include <algorithm>

const int rows = 3;
const int cols = 2;
int a[rows][cols] = {{3, 9}, {1, 7}, {2, 8}};

bool cmp(const int (&x)[cols], const int (&y)[cols]) {
    return x[0] < y[0];
}

std::sort(a, a + rows, cmp);
```

这里按每一行的第 `0` 列升序排列。

## 选择写法

| 目标 | 写法 |
| --- | --- |
| 每行分别排序 | `std::sort(a[i], a[i] + cols)` |
| 每列分别排序 | 取列到一维容器，排序后写回 |
| 全部元素排序 | `std::sort(&a[0][0], &a[0][0] + rows * cols)` |
| 按行整体排序 | `std::sort(a, a + rows, compare)` |

## 如何验证

用 `{{3, 1}, {2, 4}}` 分别执行“逐行”“逐列”“整体”和“按行”四种写法。逐行结果应为 `{{1, 3}, {2, 4}}`，逐列结果应为 `{{2, 1}, {3, 4}}`。先写出期望矩阵，才能确认自己调用的是哪一种排序。

## 注意

1. `sort` 的结束位置不包含在区间中。
2. 行排序、列排序和整体排序解决的是不同问题。
3. 列元素不连续，不能直接按一维数组方式排序。
4. 动态二维数组、指针数组与内置二维数组的内存布局可能不同，不能盲目展平。

## 关联

二维数组按列排序：vector、array 与间接排序 · 排序总览：算法选择与复杂度 · std sort：区间、比较器与头文件 · 数组：基本定义与语法
