---
title: K05 数组、字符数组与字符串
tags:
  - c
  - array
  - string
  - embedded
created: 2026-10-01
status: PC 实例已编译运行
---

# K05 数组、字符数组与字符串

## 一句话结论

**C 里没有字符串类型，只有「以 `\0` 结尾的字符数组」。** 所有字符串问题的根源都是这一条：容量要留一个字节放 `\0`，传数组时必须额外传长度。

## 三条硬规则

### 一、容量 = 可见字符数 + 1

```c
char word[] = "hello";   /* 实测 sizeof = 6：5 个字母 + 1 个 \0 */
```

少算这一个字节，就是越界的开始。

### 二、复制后必须确认有 `\0`

`strncpy` 在复制满容量时**不会**补 `\0`。实测里 gcc 直接给了警告：

```text
warning: 'strncpy' output truncated before terminating nul copying 5 bytes from a string of the same length
```

正确做法是复制 `容量-1` 个字节，再手动补 `\0`。

### 三、数组传给函数后，`sizeof` 就失效了

数组传参会退化成指针，函数里 `sizeof(参数)` 得到的是指针大小。所以长度必须单独传。

## 实测输出（PC 侧已跑通）

见 [嵌入式实例.md](嵌入式实例.md)，节选：

```text
1) "hello" 占 6 字节（5 个字母 + 1 个 \0）
2) stressed 反转后 = desserts
3) "Liquid-crystal display" -> LCD
4) 修正后：ABCD（长度 4，安全）
```

## 素材（随库携带，搬走后仍可用）

| 素材 | 位置 |
| --- | --- |
| `reverse-string` 题面 / 参考解 / 接口 | [题面](../../99-素材库/exercism-c/K05-数组与字符数组/reverse-string_题面.md) · [参考解](../../99-素材库/exercism-c/K05-数组与字符数组/reverse-string_参考解.c) · [接口](../../99-素材库/exercism-c/K05-数组与字符数组/reverse-string_接口.h) |
| `acronym` 题面 / 参考解 / 接口 | [题面](../../99-素材库/exercism-c/K05-数组与字符数组/acronym_题面.md) · [参考解](../../99-素材库/exercism-c/K05-数组与字符数组/acronym_参考解.c) · [接口](../../99-素材库/exercism-c/K05-数组与字符数组/acronym_接口.h) |
| CrashCourse《数组》 | [09-数组.md](../../99-素材库/hairrrrr-C-CrashCourse/K05-数组与字符数组/09-数组.md) |
| CrashCourse《字符串》 | [14-字符串.md](../../99-素材库/hairrrrr-C-CrashCourse/K05-数组与字符数组/14-字符串.md) |

选做（题面未收录）：`word-count`、`run-length-encoding`、`isogram`、`pangram`

## 写什么

`reverse-string`、`acronym`。验收标准见 [写-练习记录.md](写-练习记录.md)

## 嵌入式落点

串口命令解析、定长接收缓冲、把收到的字节流切成字符串。PC 实例与 SPL 对照见 [嵌入式实例.md](嵌入式实例.md)

## 易错清单（本节）

1. 容量少算一个 `\0`
2. `strncpy` 复制满容量不留 `\0`
3. 数组传参后 `sizeof` 变指针大小
4. 用 `==` 比较两个字符串（比较的是地址，不是内容）
5. 遍历时写了 `<= strlen(s)`（多走一个字节）

## 关联

- 上一个：[K04 控制流与循环](../K04-控制流与循环/README.md)
- 下一个：[K06 函数、参数传递与数组传参](../K06-函数与数组传参/README.md)
- 来源：exercism/c（MIT）· hairrrrr/C-CrashCourse（本地未发现 LICENSE，本批按特批入库，许可状态不变）
