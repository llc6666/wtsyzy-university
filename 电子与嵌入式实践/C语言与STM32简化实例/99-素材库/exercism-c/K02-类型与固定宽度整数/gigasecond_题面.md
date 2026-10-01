---
title: gigasecond_题面
tags:
  - 素材
created: 2026-10-01
status: 外来素材（中文译写）
source: exercism/c（MIT 许可）
---

# 题面：十亿秒之后

> 来源：exercism/c 仓库 `exercises/practice/gigasecond`（MIT 许可）。中文译写，英文原件未保留。

任务：算出某个时刻之后**十亿秒**是哪一天、几点。

一吉秒（gigasecond）是十亿秒，也就是 1 后面跟 9 个零。

举例：如果你出生在 2015 年 1 月 24 日 22:00（晚上 10 点整），那么你满十亿秒的时刻是 2046 年 10 月 2 日 23:46:40。

## 官方签名（来自参考解）

```c
void gigasecond(time_t start, char *buffer, size_t size);
```

参考解的做法是 `start + 1000000000`，再用 `gmtime` 与 `strftime` 按 `"%Y-%m-%d %H:%M:%S"` 写进调用方给的缓冲区。

## 这一题的考点

- 十亿这个常量本身就会溢出 32 位有符号整数附近的范围（`int` 上限约 21 亿，勉强能装，但加减之后极易越界）
- `time_t` 在不同平台上的宽度不同，不能假定它是 32 位
