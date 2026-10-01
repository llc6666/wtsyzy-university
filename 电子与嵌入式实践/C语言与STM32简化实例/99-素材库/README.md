---
title: 素材库（原始资料副本）
tags:
  - source
  - material
  - index
created: 2026-10-01
status: 已收录本轮三个知识点所需素材
---

# 素材库

这里放的是**从原始资料复制来的原件**：题面、参考解、接口头文件、教程章节、示例代码。

存在的理由：小知识库会被整体搬走，不带原始下载目录。笔记里如果只写「见 `exercism-c/c-main/...`」，搬走之后就是死链。所以做题要用到的东西，在这里都有一份副本。

## 目录规则

```text
99-素材库/
├── exercism-c/                   按知识点分目录
│   ├── LICENSE.md                原仓库 MIT 许可全文（搬运时随行）
│   ├── K01-编译与最小C程序/
│   ├── K03-位运算与掩码/
│   └── K07-指针与地址/
└── hairrrrr-C-CrashCourse/
    ├── K01-编译与最小C程序/
    ├── K03-位运算与掩码/
    └── K07-指针与地址/
```

命名规则：

| 后缀 | 内容 | 来源 |
| --- | --- | --- |
| `*_题面.md` | 题目的完整说明，已把同题的背景补充合并进来 | exercism 的 `.docs/instructions.md` 与 `.docs/introduction.md` |
| `*_参考解.c` | 官方参考实现 | exercism 的 `.meta/example.c` |
| `*_接口.h` | 题目给定的函数签名 | 练习目录下的 `.h` |
| 原文件名 | 教程章节或示例代码 | CrashCourse 的 `content/` 与 `Coding/` |

## 来源与许可（事实记录）

| 来源 | 许可 | 许可文件 |
| --- | --- | --- |
| exercism/c | MIT，Copyright 2021 Exercism | 已随行：`exercism-c/LICENSE.md` |
| hairrrrr/C-CrashCourse | 原仓库内未发现 LICENSE 文件；用户 2026-09-30 授权使用并注明来源 | 无 |
| cpq/bare-metal-programming-guide | MIT，Copyright 2022 Cesanta Software Limited | 本轮未收录素材 |
| Amuvin/STM32F103-Notes | 原仓库内未发现 LICENSE 文件 | 无 |
| dekuNukem/STM32_tutorials | 未核实 | 无 |
| STMicroelectronics/STM32CubeF1 | ST 专有许可，条款未核实；仅本地学习 | 无 |

许可状态只作事实记录，如何使用由本人决定；若对外分发，建议至少保留来源与许可声明。登记详见 [00-索引与说明/资源来源登记.md](../00-索引与说明/资源来源登记.md)。

## 关于测试文件

**不自带** exercism 的 `test_*.c` 与 unity 测试框架。原因：测试框架单份 241 KB，85 题重复塞一份，搬走时又用不上原始目录的 makefile。

替代做法：每个知识点目录里的 `写-练习记录.md` 为每题写了一份**验收标准**——函数签名、用例表（输入 → 期望输出）、五类边界、必须成立的不变量。拿这份标准可以让任意 AI 或自己生成测试文件。

想跑原版测试：回到原始目录 `exercism-c/c-main/exercises/practice/<题名>/` 执行 `make test`。

## 与知识点的对应

| 知识点 | 素材位置 | 内容 |
| --- | --- | --- |
| K01 | [exercism-c/K01-编译与最小C程序/](exercism-c/K01-编译与最小C程序/) | `hello-world`、`two-fer` 题面与参考解；示例 makefile |
| K01 | [hairrrrr-C-CrashCourse/K01-编译与最小C程序/](hairrrrr-C-CrashCourse/K01-编译与最小C程序/) | `01-C语言概论.md` |
| K03 | [exercism-c/K03-位运算与掩码/](exercism-c/K03-位运算与掩码/) | `eliuds-eggs`、`allergies`、`binary`、`secret-handshake` 题面与参考解 |
| K03 | [hairrrrr-C-CrashCourse/K03-位运算与掩码/](hairrrrr-C-CrashCourse/K03-位运算与掩码/) | `21-底层程序设计.md`（位运算符全章，也是 K10 的 `volatile` 一节）、`单身狗问题_SingleDog.c` |
| K02 | [exercism-c/K02-类型与固定宽度整数/](exercism-c/K02-类型与固定宽度整数/) | `grains`、`gigasecond` 题面与参考解 |
| K02 | [hairrrrr-C-CrashCourse/K02-类型与固定宽度整数/](hairrrrr-C-CrashCourse/K02-类型与固定宽度整数/) | `05-基本类型.md` |
| K04 | [exercism-c/K04-控制流与循环/](exercism-c/K04-控制流与循环/) | `leap`、`raindrops` 题面与参考解 |
| K04 | [hairrrrr-C-CrashCourse/K04-控制流与循环/](hairrrrr-C-CrashCourse/K04-控制流与循环/) | `07-选择语句.md`、`08-循环.md` |
| K05 | [exercism-c/K05-数组与字符数组/](exercism-c/K05-数组与字符数组/) | `reverse-string`、`acronym` 题面、参考解与接口 |
| K05 | [hairrrrr-C-CrashCourse/K05-数组与字符数组/](hairrrrr-C-CrashCourse/K05-数组与字符数组/) | `09-数组.md`、`14-字符串.md` |
| K06 | [exercism-c/K06-函数与数组传参/](exercism-c/K06-函数与数组传参/) | `difference-of-squares`、`darts`、`triangle` 题面与参考解 |
| K06 | [hairrrrr-C-CrashCourse/K06-函数与数组传参/](hairrrrr-C-CrashCourse/K06-函数与数组传参/) | `10-函数.md` |
| K07 | [exercism-c/K07-指针与地址/](exercism-c/K07-指针与地址/) | `linked-list`、`list-ops`、`circular-buffer` 题面与参考解；链表与列表接口 |
| K07 | [hairrrrr-C-CrashCourse/K07-指针与地址/](hairrrrr-C-CrashCourse/K07-指针与地址/) | `12-指针.md`、`13-指针和数组.md` |
| K08 | [exercism-c/K08-结构体与内存布局/](exercism-c/K08-结构体与内存布局/) | `complex-numbers`、`clock` 题面与参考解 |
| K08 | [hairrrrr-C-CrashCourse/K08-结构体与内存布局/](hairrrrr-C-CrashCourse/K08-结构体与内存布局/) | `17-结构&联合&枚举.md` |
| K09 | [exercism-c/K09-动态内存与所有权/](exercism-c/K09-动态内存与所有权/) | `binary-search-tree` 题面与参考解 |
| K09 | [hairrrrr-C-CrashCourse/K09-动态内存与所有权/](hairrrrr-C-CrashCourse/K09-动态内存与所有权/) | `动态内存管理.md`、`5分钟看懂什么是 malloc.md` |
| K10 | 无外部素材（`volatile` 无对应练习） | 原理见 K03 目录下的 `21-底层程序设计.md`；证据由本库汇编探针给出 |
| K11 | [exercism-c/K11-预处理器与多文件/](exercism-c/K11-预处理器与多文件/) | 《链接属性与 static》《存储类别说明符》两份官方概念文档（已译中文） |
| K11 | [hairrrrr-C-CrashCourse/K11-预处理器与多文件/](hairrrrr-C-CrashCourse/K11-预处理器与多文件/) | `15-预处理器.md`、`16-编写大型程序.md`、`19-声明.md` |
| K12 | [exercism-c/K12-基础数据结构与算法/](exercism-c/K12-基础数据结构与算法/) | `binary-search`、`spiral-matrix` 题面与参考解 |
| K12 | [hairrrrr-C-CrashCourse/K12-基础数据结构与算法/](hairrrrr-C-CrashCourse/K12-基础数据结构与算法/) | `20-程序设计.md`、`23-标准库.md` |

## 补充规则

1. 新增知识点时，同步把该知识点要用的题面与参考解复制进来，并在知识点 README 里写出指向这里的具体文件名。
2. 参考解**单独存放**，与练习记录分开：先自己写完，再看参考解对照思路。
3. 任何素材都要能追溯到来源仓库与相对路径，写在对应知识点笔记的「来源」小节里。
