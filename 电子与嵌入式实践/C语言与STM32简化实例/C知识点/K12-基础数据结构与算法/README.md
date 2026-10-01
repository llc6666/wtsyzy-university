---
title: K12 基础数据结构与算法
tags:
  - c
  - algorithm
  - data-structure
  - ring-buffer
created: 2026-10-01
status: PC 实例已编译运行
---

# K12 基础数据结构与算法

## 一句话结论

**嵌入式常用的就三样：二分查找（查表）、环形缓冲（收发数据）、排序（偶尔用）。** 真正难的不是算法本身，而是边界与「满了/空了怎么表示」。

## 一、二分查找：先把区间定死

```c
size_t low = 0;
size_t high = len;                          /* 前闭后开 [low, high) */
while (low < high) {
    size_t mid = low + (high - low) / 2;    /* 不写成 (low + high) / 2，避免溢出 */
    if (arr[mid] < value) low = mid + 1;
    else                  high = mid;
}
```

实测（见 [嵌入式实例.md](嵌入式实例.md)）：

```text
1) 找 23：下标 4
1) 找  4：下标 0
1) 找 32：下标 6
1) 找  5：不在数组中
```

要点：**区间不变量一旦选定，中途不能变**。前闭后开时循环条件是 `low < high`，两端都闭时是 `low <= high`，混用就会漏掉边界元素或死循环。

## 二、环形缓冲：串口收发的主力

```c
typedef struct {
    uint8_t buf[RB_SIZE];
    size_t  head;    /* 写指针 */
    size_t  tail;    /* 读指针 */
    size_t  count;   /* 元素个数：用它区分满与空 */
} ringbuf_t;
```

实测（容量 8，写入 0..9）：

```text
2) 写入 0..9（容量只有 8）：[满][满]
   当前元素个数 = 8
   读出全部：0 1 2 3 4 5 6 7
   读完后个数 = 0，再读一次返回 失败
```

关键设计：**用 `count` 区分满与空**。

如果只靠 `head == tail`，那么「空」和「满」长得一样，分不开。三种解法：

| 做法 | 代价 |
| --- | --- |
| 额外记 `count` | 多一个字段，最直观（本例采用） |
| 永远留一个空位 | 浪费一格，但只用两个指针 |
| 记一个「满」标志 | 需要额外维护一致性 |

## 三、排序：直接用 `qsort`

```c
static int cmp_int(const void *a, const void *b)
{
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);      /* 不用 x - y，避免溢出 */
}
```

实测：`qsort` 结果 `1 2 5 7 9`。

两个要点：

- 比较函数收的是 `const void *`，要先转回真实类型再取值
- 不要写 `return x - y;`（可能溢出）
- 对结构体排序时，比较函数里按关键字逐级比较

裸机上用 `qsort` 要注意：它内部可能递归，栈很小时要谨慎；数据量小（几十个）时手写插入排序反而更省。

## 素材（随库携带，搬走后仍可用）

| 素材 | 位置 |
| --- | --- |
| `binary-search` 题面 / 参考解 | [题面](../../99-素材库/exercism-c/K12-基础数据结构与算法/binary-search_题面.md) · [参考解](../../99-素材库/exercism-c/K12-基础数据结构与算法/binary-search_参考解.c) |
| `spiral-matrix` 题面 / 参考解 | [题面](../../99-素材库/exercism-c/K12-基础数据结构与算法/spiral-matrix_题面.md) · [参考解](../../99-素材库/exercism-c/K12-基础数据结构与算法/spiral-matrix_参考解.c) |
| CrashCourse《程序设计》 | [20-程序设计.md](../../99-素材库/hairrrrr-C-CrashCourse/K12-基础数据结构与算法/20-程序设计.md) |
| CrashCourse《标准库》 | [23-标准库.md](../../99-素材库/hairrrrr-C-CrashCourse/K12-基础数据结构与算法/23-标准库.md) |

环形缓冲的完整题面见 [K07 素材目录](../../99-素材库/exercism-c/K07-指针与地址/circular-buffer_题面.md)

## 写什么

`binary-search`、`spiral-matrix`。验收标准见 [写-练习记录.md](写-练习记录.md)

## 嵌入式落点

串口收发的环形缓冲、查表（二分）、采样数据的中值/均值处理。PC 实例与 SPL 对照见 [嵌入式实例.md](嵌入式实例.md)

## 易错清单（本节）

1. 二分查找的区间写法中途改变
2. `mid = (low + high) / 2` 溢出
3. 环形缓冲只靠 `head == tail` 区分满与空
4. `qsort` 比较函数写 `return x - y;`
5. 对无序数组直接二分

## 关联

- 上一个：[K11 预处理器、头文件与多文件](../K11-预处理器与多文件/README.md)
- 本知识点是 K05（数组）、K07（指针）、K09（内存）的综合运用
- 来源：exercism/c（MIT）· hairrrrr/C-CrashCourse（本地未发现 LICENSE，本批按特批入库，许可状态不变）
