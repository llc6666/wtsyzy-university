---
title: spiral-matrix_题面
tags:
  - 素材
created: 2026-10-01
status: 外来素材（中文译写）
source: exercism/c（MIT 许可）
---

# 题面：螺旋矩阵

> 来源：exercism/c 仓库 `exercises/practice/spiral-matrix`（MIT 许可）。中文译写，英文原件未保留。

任务：生成一个给定大小的**方阵**。

矩阵用自然数填充：从左上角的 1 开始，按**顺时针、向内螺旋**的顺序递增。

## 示例

大小为 3：

```text
1 2 3
8 9 4
7 6 5
```

大小为 4：

```text
 1  2  3 4
12 13 14 5
11 16 15 6
10  9  8 7
```

## 官方签名（来自参考解）

```c
spiral_matrix_t *spiral_matrix_create(int size);
```

参考解用 `calloc` 分配矩阵结构，并把 `size == 0` 作为特殊情形处理（返回一个 size 为 0 的矩阵，而不是 NULL）。

## 这一题的考点

- 二维数组的边界：上、下、左、右四条边各走一趟，然后向内收缩
- 每一圈的起点与终点：收缩时边界如何变化，最容易差一
- `size == 0` 与 `size == 1` 两个边界
- 结果由调用方释放（函数分配）
