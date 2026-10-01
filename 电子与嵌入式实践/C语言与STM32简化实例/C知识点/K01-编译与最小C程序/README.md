---
title: K01 编译流程与最小 C 程序
tags:
  - c
  - build
  - gcc
created: 2026-09-30
status: 样板，PC 实例已编译运行
---

# K01 编译流程与最小 C 程序

## 一句话结论

C 程序从 `.c` 到可执行文件要过**预处理 → 编译 → 汇编 → 链接**四步；前三步各管一个「翻译」环节，最后一步解决「你用的函数体在哪」。

## 这个知识点在嵌入式里解决什么

交叉编译时多了一层，但四步不变：STM32 上同样是从 `.c` 生成 `.o`，再由链接器按链接脚本摆到 Flash/RAM 地址上，最后转成 `.hex/.bin` 烧进去。

理解四步的直接收益：看到 `undefined reference` 知道是链接阶段的锅（不是编译错），看到头文件改了没生效知道是重编范围的问题。

## 四步在做什么（本机实测）

本机编译器：`gcc.exe (Rev3, Built by MSYS2 project) 16.2.0`，路径 `本地 GCC 安装路径`。示例文件 `hello.c`（本目录下，514 字节）。

| 阶段 | 命令 | 产物 | 实测大小 | 这一步干了什么 |
| --- | --- | --- | --- | --- |
| 预处理 | `gcc -std=c17 -E hello.c -o hello.i` | `hello.i` | 90368 B | 展开 `#include`、替换宏、删注释 |
| 编译 | `gcc -std=c17 -S hello.i -o hello.s` | `hello.s` | 1050 B | 把 C 翻译成汇编 |
| 汇编 | `gcc -std=c17 -c hello.s -o hello.o` | `hello.o` | 1042 B | 把汇编翻译成机器码（目标文件） |
| 链接 | 见下 | `hello.exe` | — | 把 `hello.o` 和库里的 `printf` 拼起来 |

预处理后原文里的 `GREET` 和 `LED_PIN` 已经没了，直接变成字面量：

```c
printf("%s: sum=%d pin=%d\n", "hello", sum, 5);
```

汇编产物开头能看到字符串被放进只读段 `.rdata`，`main` 被标为全局符号：

```asm
.file	"k01.c"
.section .rdata,"dr"
.LC0:
	.ascii "hello\0"
.globl	main
```

最后一步（一次到位，省掉中间文件）：

```bash
gcc -std=c17 -Wall -Wextra -Wpedantic hello.c -o hello.exe
```

实测运行输出：

```text
hello: sum=5 pin=5
```

## 读什么

- 素材：`hello-world` 的 makefile（`-std=c99 -Wall -Wextra -pedantic -Werror`，比上面更严的一套开关，值得照抄习惯），副本见 [素材库](../../99-素材库/exercism-c/K01-编译与最小C程序/)；原始目录为 `exercism-c/c-main/exercises/practice/hello-world/`（随库搬运后失效）。详细分析见 [读-示例分析.md](读-示例分析.md)
- 教程：CrashCourse 的 `01-C语言概论.md` 讲的是历史与优缺点，不含编译流程，本知识点的编译部分以 gcc 实操为准

## 素材（随库携带，搬走后仍可用）

本知识点用到的原件都在 [99-素材库](../../99-素材库/README.md) 里有一份副本，不需要回到原始下载目录：

| 素材 | 位置 |
| --- | --- |
| `hello-world` 题面 | [hello-world_题面.md](../../99-素材库/exercism-c/K01-编译与最小C程序/hello-world_题面.md) |
| `hello-world` 参考解 | [hello-world_参考解.c](../../99-素材库/exercism-c/K01-编译与最小C程序/hello-world_参考解.c) |
| 示例 makefile（那套 `-Werror` 开关） | [hello-world_makefile.txt](../../99-素材库/exercism-c/K01-编译与最小C程序/hello-world_makefile.txt) |
| `two-fer` 题面 | [two-fer_题面.md](../../99-素材库/exercism-c/K01-编译与最小C程序/two-fer_题面.md) |
| `two-fer` 接口 | [two-fer_接口.h](../../99-素材库/exercism-c/K01-编译与最小C程序/two-fer_接口.h) |
| `two-fer` 参考解 | [two-fer_参考解.c](../../99-素材库/exercism-c/K01-编译与最小C程序/two-fer_参考解.c) |
| CrashCourse《C语言概述》 | [01-C语言概论.md](../../99-素材库/hairrrrr-C-CrashCourse/K01-编译与最小C程序/01-C语言概论.md) |

## 写什么

- `hello-world`：改骨架里的返回值，让测试通过
- `two-fer`：字符串拼装的入门题

进度与复盘见 [写-练习记录.md](写-练习记录.md)

## 嵌入式落点

同样的四步在 ARM-GCC 上跑一遍，最后多一个「格式转换」：`arm-none-eabi-gcc` 编译链接出 `.elf`，再用 `objcopy` 转 `.hex/.bin` 烧录。SPL 工程里除用户 `.c` 外，还会链接启动文件、库源文件和标准外设驱动。

PC 可编译实例与 SPL 对照见 [嵌入式实例.md](嵌入式实例.md)

## 易错清单（本节）

完整清单见 [00-索引与说明/易错清单.md](../../00-索引与说明/易错清单.md)，本节相关的四条：

1. 改了 `.h` 行为没变 → 头文件改动没触发重编，clean 后再编
2. `undefined reference` → 链接阶段找不到函数体，不是语法错
3. `implicit declaration` → 用了没声明的函数
4. 局部变量随机值 → 未初始化，栈上残留

## 关联

- 上一个：无（起点）
- 下一个：[K03 位运算与掩码](../K03-位运算与掩码/README.md)
- 来源：exercism/c（MIT）· hairrrrr/C-CrashCourse（本地未发现 LICENSE，本批按特批入库，许可状态不变）
