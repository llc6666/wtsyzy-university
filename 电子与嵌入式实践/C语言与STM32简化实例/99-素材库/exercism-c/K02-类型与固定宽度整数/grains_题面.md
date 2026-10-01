---
title: grains_题面
tags:
  - 素材
created: 2026-10-01
status: 外来素材（中文译写）
source: exercism/c（MIT 许可）
---

# 题面：棋盘麦粒

> 来源：exercism/c 仓库 `exercises/practice/grains`（MIT 许可）。中文译写，英文原件未保留。

算一算棋盘上要放多少粒麦子。

棋盘有 64 格。第 1 格放 1 粒，第 2 格放 2 粒，第 3 格放 4 粒，以此类推，每格翻一倍。

写出代码计算：

- 指定某一格上有多少粒麦子
- 整个棋盘上一共有多少粒麦子

## 官方签名（来自参考解）

```c
uint64_t square(uint8_t index);   // 第 index 格（1..64），越界返回 0
uint64_t total(void);             // 全部 64 格的总和
```

## 这一题的考点

第 64 格的数是 2⁶³，总和是 2⁶⁴ − 1 —— 这两个数都**装不进 `int`，也装不进 `uint32_t`**。用错类型会静默溢出，这正是 K02 要练的东西。
