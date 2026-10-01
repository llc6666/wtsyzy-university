---
title: clock_题面
tags:
  - 素材
created: 2026-10-01
status: 外来素材（中文译写）
source: exercism/c（MIT 许可）
---

# 题面：时钟

> 来源：exercism/c 仓库 `exercises/practice/clock`（MIT 许可）。中文译写，英文原件未保留。

实现一个只管时间、不管日期的时钟。

要能对它加减分钟。

两个表示同一时刻的时钟应当**相等**。

## 官方实现要点（来自参考解）

```c
static const char CLOCK_FORMAT[] = "%02d:%02d";

static void normalize_clock(int *hour, int *minute)
{
   while (*minute < 0) { *minute += 60; *hour -= 1; }
   *hour += *minute / 60;
   /* …继续把 hour 归一到 0..23，minute 归一到 0..59… */
}
```

两个关键点：

1. `normalize_clock` 用**指针参数**同时改 hour 和 minute —— 这就是 K06「要改调用方的数据就得传地址」的实例
2. 负数的分钟用 `while` 循环不断加 60 直到非负，而不是用取模（C 里负数取模的结果依实现而定）

## 这一题的考点

- 时钟的「相等」不是比较结构体每个字段，而是比较归一化后的时刻
- 加减分钟后要绕回（24 小时制）
- 负数处理：减 90 分钟会跨过零点
