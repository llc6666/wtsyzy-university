---
title: two-fer_题面
tags:
  - 素材
created: 2026-10-01
status: 外来素材（中文译写）
source: exercism/c（MIT 许可）
---

# 题面：Two-Fer（分饼干）

> 来源：exercism/c 仓库 `exercises/practice/two-fer`（MIT 许可）。本文为中文译写，英文原件未保留。

任务：决定送出多余那块饼干时你要说什么。

如果知道对方的名字（比如叫 Do-yun），就说：

```text
One for Do-yun, one for me.
```

如果不知道对方的名字，就用 you 代替：

```text
One for you, one for me.
```

示例：

| 名字 | 要说的话 |
| --- | --- |
| Alice | `One for Alice, one for me.` |
| Bohdan | `One for Bohdan, one for me.` |
| （不知道名字） | `One for you, one for me.` |
| Zaphod | `One for Zaphod, one for me.` |

## 官方测试给出的两条关键信息

1. 「不知道名字」用 `name == NULL` 表示，不是空字符串
2. 结果写进调用方提供的缓冲区，官方测试用的缓冲区是 100 字节

（这两条来自官方测试文件，题面原文没写，但对做题是必要的。）
