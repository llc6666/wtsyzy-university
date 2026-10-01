---
title: darts_题面
tags:
  - 素材
created: 2026-10-01
status: 外来素材（中文译写）
source: exercism/c（MIT 许可）
---

# 题面：飞镖得分

> 来源：exercism/c 仓库 `exercises/practice/darts`（MIT 许可）。中文译写，英文原件未保留。

算一次投掷飞镖得了多少分。

在这个简化版规则里，靶子按落点给四种分数（原题是 SVG 示意图，这里用文字描述）：

- 落在靶子**外面**：0 分
- 落在**外圈**：1 分
- 落在**中圈**：5 分
- 落在**内圈**：10 分

圆的半径：外圈半径 10 个单位（也是整个靶子的半径），中圈半径 5，内圈半径 1。它们**圆心相同**，都在坐标 (0, 0)。

给定落点的直角坐标 `x` 和 `y`（都是实数），算出这次投掷的得分。

## 官方实现里的常量（来自参考解）

```c
static const float INNER_CIRCLE_RADIUS  = 1;
static const float MIDDLE_CIRCLE_RADIUS = 5;
static const float OUTER_CIRCLE_RADIUS  = 10;

static const uint8_t INNER_CIRCLE_SCORE  = 10;
static const uint8_t MIDDLE_CIRCLE_SCORE = 5;
static const uint8_t OUTER_CIRCLE_SCORE  = 1;
static const uint8_t OFF_BOARD_SCORE     = 0;
```

## 这一题的考点

- 落点到圆心的距离用 `sqrt(x*x + y*y)`（`<math.h>`，链接时要加 `-lm`）
- 先判内圈、再中圈、再外圈，顺序不能反
- 浮点比较：落在边界上（距离正好等于 10）算不算命中，要按题意确定
