---
title: raindrops_题面
tags:
  - 素材
created: 2026-10-01
status: 外来素材（中文译写）
source: exercism/c（MIT 许可）
---

# 题面：雨滴声

> 来源：exercism/c 仓库 `exercises/practice/raindrops`（MIT 许可）。中文译写，英文原件未保留。

任务：把一个数转换成对应的雨滴声音。

对给定的数：

- 能被 3 整除，结果加上 `Pling`
- 能被 5 整除，结果加上 `Plang`
- 能被 7 整除，结果加上 `Plong`
- 如果 3、5、7 **都**不能整除，结果就是这个数本身的字符串形式

## 示例

- 28 能被 7 整除，不能被 3 或 5 整除 → `"Plong"`
- 30 能被 3 和 5 整除，不能被 7 整除 → `"PlingPlang"`
- 34 三个都除不尽 → `"34"`

## 官方签名（来自参考解）

```c
void convert(char result[], int drops);
```

结果写进调用方给的 `result`；参考解用 `strcat` 追加，全部落空时用 `snprintf` 写入数字。

## 这一题的考点

三个条件**不是互斥**的（30 同时命中两条），所以不能用 `else if` 串成一条链——这是 K04 分支结构最容易写错的地方。
