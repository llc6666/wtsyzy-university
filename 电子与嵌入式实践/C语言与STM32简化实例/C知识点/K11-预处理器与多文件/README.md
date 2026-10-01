---
title: K11 预处理器、头文件与多文件
tags:
  - c
  - preprocessor
  - header
  - multi-file
created: 2026-10-01
status: PC 多文件实例已编译运行
---

# K11 预处理器、头文件与多文件

## 一句话结论

**头文件放声明，源文件放定义；`static` 决定「别的文件看不看得见」，`extern` 表示「定义在别处」；每个 `.c` 单独编译成目标文件，最后链接成一个程序。**

## 编译与链接是两件事

```bash
gcc -std=c17 -Wall -Wextra -Wpedantic main.c sensor.c -o app.exe
```

这一条命令背后是：分别编译 `main.c` 与 `sensor.c`，再链接。也可以分开做：

```bash
gcc -std=c17 -c main.c   -o main.o
gcc -std=c17 -c sensor.c -o sensor.o
gcc main.o sensor.o -o app.exe
```

分开做的好处：改了 `sensor.c` 只需重新编译它一个文件。

## 实测：本知识点的示例工程

目录 [multifile-demo/](multifile-demo/)，三个文件：`sensor.h`、`sensor.c`、`main.c`。编译运行结果：

```text
1) 读到数值：2468
2) 读取次数（来自另一个文件的全局变量）：3
3) 传空指针的返回值：-1（非 0 表示失败）
```

## 三条规则

### 一、头文件只放声明，不放定义

```c
#ifndef SENSOR_H
#define SENSOR_H

int sensor_read(uint16_t *out_value);   /* 声明 */
extern uint32_t g_sensor_read_count;    /* 声明：定义在别处 */
#endif
```

`ifndef/define/endif` 是 **include guard**：同一个头文件被包含两次时，第二次整段跳过，避免重复声明报错。

### 二、`static` = 只在本文件可见

`sensor.c` 里的 `static uint16_t s_last_raw` 与 `static uint16_t scale_raw()` 只属于那个文件。

**实测验证**：在另一个文件里写 `extern uint16_t s_last_raw;` 去引用它，链接阶段失败（退出码非 0）。具体提示文本因工具链而异，但结论一致：内部链接的标识符，别的翻译单元拿不到。

好处：不同 `.c` 文件里可以有同名的内部函数，互不干扰。

### 三、`extern` 是「声明」，定义只有一处

头文件里写 `extern uint32_t g_sensor_read_count;`，**定义**写在 `sensor.c` 里：

```c
uint32_t g_sensor_read_count = 0;
```

## 素材（随库携带，搬走后仍可用）

| 素材 | 位置 |
| --- | --- |
| 概念《链接属性与 static》 | [链接属性与static_概念.md](../../99-素材库/exercism-c/K11-预处理器与多文件/链接属性与static_概念.md) |
| 概念《存储类别说明符》 | [存储类别说明符_概念.md](../../99-素材库/exercism-c/K11-预处理器与多文件/存储类别说明符_概念.md) |
| CrashCourse《预处理器》 | [15-预处理器.md](../../99-素材库/hairrrrr-C-CrashCourse/K11-预处理器与多文件/15-预处理器.md) |
| CrashCourse《编写大型程序》 | [16-编写大型程序.md](../../99-素材库/hairrrrr-C-CrashCourse/K11-预处理器与多文件/16-编写大型程序.md) |
| CrashCourse《声明》 | [19-声明.md](../../99-素材库/hairrrrr-C-CrashCourse/K11-预处理器与多文件/19-声明.md) |

## 写什么

两道自建练习：把单文件拆成多文件；验证 `static`/`extern` 的可见性。验收标准见 [写-练习记录.md](写-练习记录.md)

## 嵌入式落点

`stm32f10x.h` 的组织方式、驱动模块化（一个外设一对 `.c/.h`）、头文件里的寄存器映射、条件编译选择芯片型号。PC 实例与 SPL 对照见 [嵌入式实例.md](嵌入式实例.md)

## 易错清单（本节）

1. 头文件里写了变量定义 → 被多个 `.c` 包含时重复定义
2. 忘记 include guard
3. 宏参数不加括号 → 展开后优先级被改写
4. 在头文件里声明了 `static` 变量（见概念文档里的陷阱）
5. 声明与定义的类型不一致 → 编译期可能不报错，链接期或运行期出错

## 关联

- 上一个：[K10 volatile 与编译器优化](../K10-volatile与编译器优化/README.md)
- 下一个：[K12 基础数据结构与算法](../K12-基础数据结构与算法/README.md)
- 来源：exercism/c（MIT）· hairrrrr/C-CrashCourse（本地未发现 LICENSE，本批按特批入库，许可状态不变）
