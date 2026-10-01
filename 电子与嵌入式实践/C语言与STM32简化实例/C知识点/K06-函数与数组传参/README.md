---
title: K06 函数、参数传递与数组传参
tags:
  - c
  - function
  - arguments
  - embedded
created: 2026-10-01
status: PC 实例已编译运行
---

# K06 函数、参数传递与数组传参

## 一句话结论

**C 只有值传递。** 想让函数改到外面的数据，就把那个数据的**地址**传进去；数组传进去会退化成指针，所以长度必须另传。

## 三条要记牢

### 一、值传递改的是副本

实测：

```text
1) 值传递后：x=1 y=2（没换）
   指针传递后：x=2 y=1（换了）
```

### 二、数组传参后长度信息丢失

```text
2) 在本函数里 sizeof(data)=20（整个数组）
   传给函数后，函数里只能拿到指针，所以必须另传 len=5
```

`void f(int arr[])` 与 `void f(int *arr)` **完全等价**，编译器都当成指针。所以签名里必须带 `size_t len`。

### 三、需要带回多个结果时，用指针参数

```c
void min_max(const int *arr, size_t len, int *out_min, int *out_max);
```

实测：`最小值=1 最大值=9`。

## `const` 的两种位置

| 写法 | 含义 |
| --- | --- |
| `const int *p` | 不能通过 `p` 改数据，`p` 可以指向别处（**数据只读**） |
| `int *const p` | `p` 不能再指向别处，但能改它指向的数据（**指针只读**） |

给不需要修改的参数加 `const`，既是给调用方的承诺，也能让编译器帮你查出误改。

## 素材（随库携带，搬走后仍可用）

| 素材 | 位置 |
| --- | --- |
| `difference-of-squares` 题面 / 参考解 | [题面](../../99-素材库/exercism-c/K06-函数与数组传参/difference-of-squares_题面.md) · [参考解](../../99-素材库/exercism-c/K06-函数与数组传参/difference-of-squares_参考解.c) |
| `darts` 题面 / 参考解 | [题面](../../99-素材库/exercism-c/K06-函数与数组传参/darts_题面.md) · [参考解](../../99-素材库/exercism-c/K06-函数与数组传参/darts_参考解.c) |
| `triangle` 题面 / 参考解 | [题面](../../99-素材库/exercism-c/K06-函数与数组传参/triangle_题面.md) · [参考解](../../99-素材库/exercism-c/K06-函数与数组传参/triangle_参考解.c) |
| CrashCourse《函数》 | [10-函数.md](../../99-素材库/hairrrrr-C-CrashCourse/K06-函数与数组传参/10-函数.md) |

## 写什么

`difference-of-squares`、`darts`、`triangle`。验收标准见 [写-练习记录.md](写-练习记录.md)

## 嵌入式落点

驱动函数的参数约定（先传地址再填值）、数组与长度成对出现、用 `const` 保护只读数据、用返回值表达成功失败。PC 实例与 SPL 对照见 [嵌入式实例.md](嵌入式实例.md)

## 易错清单（本节）

1. 以为传了变量函数就能改它 → 必须传地址
2. 数组参数忘了带长度
3. `sizeof(数组参数)` 得到指针大小
4. `const` 位置写错，保护了不该保护的东西
5. 返回局部变量的地址

## 关联

- 上一个：[K05 数组、字符数组与字符串](../K05-数组与字符数组/README.md)
- 下一个：[K07 指针与地址](../K07-指针与地址/README.md)
- 来源：exercism/c（MIT）· hairrrrr/C-CrashCourse（本地未发现 LICENSE，本批按特批入库，许可状态不变）
