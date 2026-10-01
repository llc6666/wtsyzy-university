---
title: difference-of-squares_题面
tags:
  - 素材
created: 2026-10-01
status: 外来素材（中文译写）
source: exercism/c（MIT 许可）
---

# 题面：平方和与和的平方之差

> 来源：exercism/c 仓库 `exercises/practice/difference-of-squares`（MIT 许可）。中文译写，英文原件未保留。

求前 N 个自然数的「和的平方」与「平方和」之差。

前十个自然数的和的平方是：

`(1 + 2 + ... + 10)² = 55² = 3025`

前十个自然数的平方和是：

`1² + 2² + ... + 10² = 385`

两者之差：

`3025 - 385 = 2640`

题面明确说明：不要求你从零推出高效解法，允许并且鼓励去查资料——找到更好的算法本身就是软件工程的关键能力。

## 官方签名（来自参考解）

```c
unsigned int sum_of_squares(unsigned int number);
unsigned int square_of_sum(unsigned int number);
unsigned int difference_of_squares(unsigned int number);
```

参考解用循环累加实现。

## 这一题的考点

- 用一个函数算出结果，再用另一个函数组合：函数的分工与复用
- 循环里的 `i <= number` 边界：包含 N 本身
- 数值上界：N 较大时 `unsigned int` 也会溢出
