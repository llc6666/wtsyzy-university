---
title: triangle_题面
tags:
  - 素材
created: 2026-10-01
status: 外来素材（中文译写）
source: exercism/c（MIT 许可）
---

# 题面：三角形类型

> 来源：exercism/c 仓库 `exercises/practice/triangle`（MIT 许可）。中文译写，英文原件未保留。

判断一个三角形是等边、等腰还是不等边。

- **等边** equilateral：三条边长度都相同
- **等腰** isosceles：至少两条边长度相同（有的定义是「恰好两条」，本题按「至少两条」处理）
- **不等边** scalene：三条边长度各不相同

## 注意：先得是个三角形

要成为三角形，必须满足：

- 三条边长度都 > 0
- 任意两边之和 ≥ 第三边

用式子表示，设三边为 `a`、`b`、`c`，以下三条必须同时成立：

```text
a + b ≥ c
b + c ≥ a
a + c ≥ b
```

## 退化三角形

**退化三角形**指两边之和**等于**第三边的情况，例如 `1, 1, 2`。本题的测试不包含这种情况，你可以处理，也可以不管。

## 官方接口（来自参考解）

```c
typedef struct { double a, b, c; } triangle_t;   /* 具体定义以头文件为准 */

bool is_equilateral(triangle_t sides);
bool is_isosceles(triangle_t sides);
bool is_scalene(triangle_t sides);
```

参考解先用一个 `triangle_equality()` 判断「能不能构成三角形」，再判断类型。

## 这一题的考点

- 结构体按值传参（三条边打包成一个 `triangle_t`）
- 等腰的定义是「至少两条相等」，所以**等边也是等腰**——判定时要注意包含关系
- 浮点比较相等要用误差范围，不能直接 `==`
