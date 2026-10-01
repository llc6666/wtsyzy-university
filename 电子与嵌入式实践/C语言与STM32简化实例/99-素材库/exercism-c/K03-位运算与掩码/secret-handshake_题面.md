---
title: secret-handshake_题面
tags:
  - 素材
created: 2026-10-01
status: 外来素材（中文译写）
source: exercism/c（MIT 许可）
---

# 题面：秘密握手

> 来源：exercism/c 仓库 `exercises/practice/secret-handshake`（MIT 许可）。本文为中文译写，英文原件未保留。

## 背景

你要和几个朋友、以及朋友的朋友一起办一个秘密编程俱乐部。不是所有人都互相认识，所以你们决定设计一套秘密握手动作，用来确认对方是不是俱乐部成员。你们把暗号设计成这样：一个人说一个 1 到 31 之间的数，另一个人把它转换成一串动作。

## 任务

把 1 到 31 之间的一个数转换成一串动作（秘密握手）。

动作序列由这个数转成二进制后的**最右边五位**决定，从最右边那一位开始往左看。

每一位对应的动作：

```text
00001 = 眨眼 wink
00010 = 快速眨两下 double blink
00100 = 闭上眼睛 close your eyes
01000 = 跳一下 jump
10000 = 把前面得到的动作顺序反转
```

以数字 `9` 为例：

- 9 的二进制是 `1001`
- 最右边一位是 1，所以第一个动作是 wink
- 往左一位是 0，所以没有 double blink
- 再往左是 0，所以不闭眼
- 再往左是 1，所以 jump

看完了，最终结果是：`wink, jump`

再以 `26` 为例，二进制 `11010`：

- double blink
- jump
- 反转动作顺序

所以 26 的秘密握手是：`jump, double blink`

## 官方接口

```c
const char **commands(size_t number);
```

返回值是指向字符串指针数组的指针，最多 4 个动作。
