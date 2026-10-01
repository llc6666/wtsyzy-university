---
title: acronym_题面
tags:
  - 素材
created: 2026-10-01
status: 外来素材（中文译写）
source: exercism/c（MIT 许可）
---

# 题面：首字母缩写

> 来源：exercism/c 仓库 `exercises/practice/acronym`（MIT 许可）。中文译写，英文原件未保留。

把一个短语转换成它的首字母缩写。

技术人员最爱三字母缩写（TLA）！写个程序把 `Portable Network Graphics` 这样的长名字变成 `PNG`。

标点处理规则：

- 连字符 `-` 视为单词分隔符（和空格一样）
- 其他标点可以从输入中去掉

示例：

| 输入 | 输出 |
| --- | --- |
| As Soon As Possible | ASAP |
| Liquid-crystal display | LCD |
| Thank George It's Friday! | TGIF |

## 官方签名（来自参考解）

```c
char *abbreviate(const char *phrase);   // 具体名字以头文件为准
```

参考解用 `isalpha` 判断字母，并把「前一个字符是空格、连字符或下划线」当作单词开头的标志。

## 这一题的考点

- 逐字符扫描字符数组，靠「前一个字符是什么」判断单词边界
- 结果要转成大写（`<ctype.h>` 的 `toupper`）
- 结果字符串要自己分配空间并留 `\0`
