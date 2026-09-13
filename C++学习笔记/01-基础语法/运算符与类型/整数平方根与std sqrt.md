---
title: 整数平方根与 std::sqrt
tags:
  - c-plus-plus
  - math
  - binary-search
  - overflow
status: public
---

# 整数平方根与 `std::sqrt`

## 一句话结论

`std::sqrt` 返回浮点近似值，适合做估计或初值。需要整数答案时，先明确目标是 `floor(sqrt(n))`，再用整数条件修正或直接使用整数二分。判断 `mid * mid` 时不要盲乘，使用 `mid <= n / mid` 可以规避乘法溢出。

## 1. 先确定答案定义

对非负整数 `n`，整数平方根通常指：

```text
r = floor(sqrt(n))
```

它满足：

```text
r * r <= n < (r + 1) * (r + 1)
```

| `n` | `sqrt(n)` 的数学值 | 整数平方根 |
| ---: | ---: | ---: |
| 0 | 0 | 0 |
| 1 | 1 | 1 |
| 2 | 1.414... | 1 |
| 15 | 3.872... | 3 |
| 16 | 4 | 4 |
| 17 | 4.123... | 4 |

若题目要求四舍五入、向上取整或浮点误差范围，判据会随之改变，不能直接套用下取整实现。

## 2. `std::sqrt` 的正确定位

```cpp
#include <cmath>

double x = std::sqrt(17.0);
```

它返回浮点类型的平方根。把结果直接转换为整数通常能处理常见输入，但严谨代码仍需考虑舍入误差。安全流程是：

1. 用 `std::sqrt` 得到估计值。
2. 用整数除法检查估计值是否偏大或偏小。
3. 调整到满足平方根定义的最后一个整数。

修正时使用 `r > n / r` 与 `r < n / (r + 1)`，避免直接计算两个可能溢出的平方。

## 3. 整数二分的不变量

把候选答案放在 `[lo, hi)` 中，维护：

- `lo` 始终是可能的答案；
- `hi` 始终大于真正答案；
- 当 `lo + 1 == hi` 时，`lo` 就是最大的可行整数。

对中点 `mid` 判断 `mid * mid <= n`。因为 `mid > 0`，它可以安全改写为：

```cpp
mid <= n / mid
```

`n == 0`、`n == 1` 先直接返回，能同时处理零除和最小边界。

## 4. 可复用实现

```cpp
#include <cmath>
#include <stdexcept>

long long integer_sqrt_binary(long long n) {
    if (n < 0) {
        throw std::invalid_argument("integer square root needs n >= 0");
    }
    if (n < 2) {
        return n;
    }

    constexpr long long root_limit = 3037000500LL;
    long long lo = 1;
    long long hi = (n < root_limit) ? (n + 1) : root_limit;

    while (lo + 1 < hi) {
        const long long mid = lo + (hi - lo) / 2;
        if (mid <= n / mid) {
            lo = mid;
        } else {
            hi = mid;
        }
    }
    return lo;
}

long long integer_sqrt_from_float(long long n) {
    if (n < 0) {
        throw std::invalid_argument("integer square root needs n >= 0");
    }
    if (n < 2) {
        return n;
    }

    long long r = static_cast<long long>(std::sqrt(static_cast<long double>(n)));
    if (r == 0) {
        r = 1;
    }
    while (r > n / r) {
        --r;
    }
    while (r < n / (r + 1)) {
        ++r;
    }
    return r;
}
```

第一种实现只依赖整数运算，适合把正确性放在首位。第二种实现适合已有浮点初值的场景，但修正循环不能删除。

## 5. 最小可编译验证

```cpp
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

long long integer_sqrt_binary(long long n) {
    if (n < 0) throw std::invalid_argument("n must be non-negative");
    if (n < 2) return n;
    constexpr long long root_limit = 3037000500LL;
    long long lo = 1;
    long long hi = (n < root_limit) ? n + 1 : root_limit;
    while (lo + 1 < hi) {
        long long mid = lo + (hi - lo) / 2;
        if (mid <= n / mid) lo = mid;
        else hi = mid;
    }
    return lo;
}

long long integer_sqrt_from_float(long long n) {
    if (n < 0) throw std::invalid_argument("n must be non-negative");
    if (n < 2) return n;
    long long r = static_cast<long long>(std::sqrt(static_cast<long double>(n)));
    if (r == 0) r = 1;
    while (r > n / r) --r;
    while (r < n / (r + 1)) ++r;
    return r;
}

int main() {
    const long long max_value = std::numeric_limits<long long>::max();
    const long long expected_max_root = 3037000499LL;
    for (long long n : {0LL, 1LL, 2LL, 3LL, 4LL, 15LL, 16LL, 17LL,
                        max_value}) {
        const long long expected = (n == max_value)
            ? expected_max_root
            : static_cast<long long>(std::sqrt(static_cast<long double>(n)));
        assert(integer_sqrt_binary(n) == expected);
        assert(integer_sqrt_from_float(n) == expected);
    }
    std::cout << "sqrt checks passed\n";
}
```

编译命令：

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -g sqrt_demo.cpp -o sqrt_demo
```

预期输出：

```text
sqrt checks passed
```

## 6. 常见错误与修复

| 错误 | 结果 | 修复 |
| --- | --- | --- |
| 对 `n == 0` 直接执行 `n / r` | `r` 可能为 0，发生除零 | 对 `n < 2` 先返回 |
| 用 `mid * mid <= n` 判断大整数 | 乘法可能溢出，比较结果失真 | 使用 `mid <= n / mid` |
| 直接把 `std::sqrt` 转成整数并宣称绝对正确 | 舍入误差可能让结果偏一 | 用整数条件修正，或采用整数二分 |
| 用 `hi = n + 1` 覆盖全部 `long long` | `n` 为最大值时 `n + 1` 溢出 | 使用安全上界，或单独处理最大值 |
| 负数输入仍返回整数平方根 | 问题定义不成立 | 明确拒绝负数或另行定义行为 |

## 关联资料

- [C++ Working Draft N4950](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/n4950.pdf)
- [GCC C++ 标准支持状态](https://gcc.gnu.org/projects/cxx-status.html)
- [二分查找：区间不变量与边界定位](../../07-基础算法/二分查找：区间不变量与边界定位.md)

## 发布边界

本文保留平方根定义、溢出规避、可编译代码和边界测试。个人题单、学习进度、私有路径和对话记录均未纳入。
