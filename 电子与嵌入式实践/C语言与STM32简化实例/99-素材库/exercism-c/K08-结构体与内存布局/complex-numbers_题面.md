---
title: complex-numbers_题面
tags:
  - 素材
created: 2026-10-01
status: 外来素材（中文译写）
source: exercism/c（MIT 许可）
---

# 题面：复数运算

> 来源：exercism/c 仓库 `exercises/practice/complex-numbers`（MIT 许可）。中文译写，英文原件未保留。

**复数**的形式是 `z = a + b * i`，其中：

- `a` 是**实部**（实数）
- `b` 是**虚部**（实数）
- `i` 是**虚数单位**，满足 `i² = -1`

## 复数的运算

### 共轭

```text
zc = a - b * i
```

### 绝对值（模）

```text
|z| = sqrt(a² + b²)
```

绝对值的平方等于复数与它的共轭的乘积：

```text
|z|² = z * zc = a² + b²
```

### 加法

```text
z1 + z2 = (a + c) + (b + d) * i
```

### 减法

```text
z1 - z2 = (a - c) + (b - d) * i
```

### 乘法

```text
z1 * z2 = (a * c - b * d) + (b * c + a * d) * i
```

### 倒数

```text
1 / z = a / (a² + b²) - b / (a² + b²) * i
```

### 除法

```text
z1 / z2 = (a * c + b * d) / (c² + d²) + (b * c - a * d) / (c² + d²) * i
```

### 指数（自然常数 e 的复指数，欧拉公式）

```text
e^(a + b * i) = e^a * (cos(b) + i * sin(b))
```

## 要求

**不许使用语言内置的复数支持**，实现以下运算：

- 加法、减法、乘法、除法
- 共轭
- 绝对值
- 以 e 为底的复指数

## 官方接口（来自参考解）

```c
typedef struct { double real; double imag; } complex_t;   /* 以头文件为准 */

complex_t c_add(const complex_t a, const complex_t b);
/* 其余运算同理 */
```

## 这一题的考点

- 结构体作为返回值与参数（按值传递）
- 每个运算返回一个新结构体，不修改输入
- 除法的分母 `c² + d²` 为零时要处理
