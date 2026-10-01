---
title: reverse-string_题面
tags:
  - 素材
created: 2026-10-01
status: 外来素材（中文译写）
source: exercism/c（MIT 许可）
---

# 题面：反转字符串

> 来源：exercism/c 仓库 `exercises/practice/reverse-string`（MIT 许可）。中文译写，英文原件未保留。

任务：把给定的字符串反转。

示例：

- `"stressed"` → `"desserts"`
- `"strops"` → `"sports"`
- `"racecar"` → `"racecar"`（回文，反转后不变）

## 官方签名（来自参考解）

```c
char *reverse(const char *value);
```

参考解 `malloc` 出一块新内存写入结果并返回，**由调用方释放**。

## 这一题的考点

- 字符数组与结尾的 `\0`：新串也要留一个字节放 `\0`
- 双指针：一个从前往后读，一个从后往前写
- 所有权：函数分配、调用方释放，要写清楚
