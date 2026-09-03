---
title: CodeForces 1876A：按传播成本排序的贪心
tags:
  - c-plus-plus
  - algorithm-competition
  - greedy
  - sorting
  - codeforces-1876A
created: 2026-09-01
status: 学习中
source: https://codeforces.com/problemset/problem/1876/A?locale=en
---

# CodeForces 1876A：按传播成本排序的贪心

## 一句话结论

先直接通知 1 人，之后按 `b_i` 从小到大使用传播容量；当 `b_i >= p` 时，剩余居民全部直接通知。实际使用人数必须写成 `min(a_i, remaining)`，总费用使用 `long long` 计算。

## 它解决什么问题

每个居民 `i` 有两个属性：

- `a_i`：这个居民最多可以通过头盔通知多少人。
- `b_i`：这个居民通过头盔通知 1 人的费用。

Pak Chanek 直接通知 1 人的费用是 `p`，目标是让全部 `n` 人收到消息，同时使总费用最小。

题目允许直接通知一个或多个人。先支付一次 `p` 把消息交给一个居民，剩下的人再比较头盔传播费用和直接通知费用。

## 工作机制

### 1. 先固定一个直接通知

至少需要有一个居民先收到消息，因此先加入一次直接费用：

```cpp
long long cost = p;
int remaining = n - 1;
```

`remaining` 表示还没有收到消息的人数。这里单独使用 `remaining`，避免把总人数 `n` 和剩余人数混成一个变量。

### 2. 按传播单价排序

```cpp
sort(s.begin(), s.end(), [](const vector<int>& x,
                            const vector<int>& y) {
    return x[1] < y[1];
});
```

先使用 `b_i` 较小的传播容量。传播 1 人的费用越低，同样的钱可以通知越多人。

### 3. 每次只取合法数量

当前居民最多只能通知 `a_i` 人，而剩余人数可能更少，因此实际传播人数为：

```cpp
int take = min(s[i][0], remaining);
```

更新状态：

```cpp
cost += 1LL * take * s[i][1];
remaining -= take;
```

当 `remaining` 变成 0，所有居民都已收到消息。

### 4. 头盔费用不低于直接费用时停止使用

因为数组已经按 `b_i` 升序排列，所以一旦出现：

```cpp
s[i][1] >= p
```

当前居民和后面的居民都没有比直接通知更便宜。剩余人数统一按 `p` 直接通知即可。

## 为什么贪心正确

### 交换论证

假设一个方案使用了费用较高的传播容量，同时还有费用更低且尚未使用的传播容量。把高费用传播替换为低费用传播：

- 通知人数不变；
- 传播限制仍然满足；
- 总费用不会增加。

重复这个替换过程后，必然可以得到“先用较低 `b_i`，再用较高 `b_i`”的最优方案。因此按 `b_i` 升序使用容量是安全的。

当 `b_i >= p` 时，头盔传播一次的费用不低于直接通知一次。将这次传播改成直接通知也不会增加费用，所以后面的居民可以全部直接通知。

## 最小可提交实现

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false); // 关闭不必要的输入输出同步
    cin.tie(nullptr);             // 解除 cin 与 cout 的绑定

    int t;
    cin >> t;                     // 读取测试用例数量

    while (t--) {
        int n, p;
        cin >> n >> p;             // n 是总人数，p 是直接通知单价

        vector<vector<int>> s(n, vector<int>(2));

        for (int i = 0; i < n; ++i) {
            cin >> s[i][0];       // 读取第 i 个居民的传播容量 a_i
        }

        for (int i = 0; i < n; ++i) {
            cin >> s[i][1];       // 读取第 i 个居民的传播单价 b_i
        }

        sort(s.begin(), s.end(),
             [](const vector<int>& x, const vector<int>& y) {
                 return x[1] < y[1]; // 传播单价低的居民排在前面
             });

        long long cost = p;        // 先直接通知 1 人
        int remaining = n - 1;     // 还没有收到消息的人数

        for (int i = 0; i < n && remaining > 0; ++i) {
            if (s[i][1] >= p) {
                cost += 1LL * remaining * p; // 头盔不便宜，剩余人直接通知
                remaining = 0;                // 所有人都已处理
                break;
            }

            int take = min(s[i][0], remaining); // 当前居民实际通知的人数
            cost += 1LL * take * s[i][1];       // 1LL 让乘法从第一步使用 long long
            remaining -= take;                  // 消耗未通知人数
        }

        cout << cost << '\n';       // 输出最小总费用
    }

    return 0;
}
```

## 本题暴露的错误

### 错误 1：把下标 `i` 当成了人数

原代码中的：

```cpp
if (i + s[i][0] <= n)
```

混合了三个不同含义：

- `i`：当前数组下标；
- `s[i][0]`：当前居民的传播容量；
- `n`：原代码执行 `n--` 后代表剩余人数。

容量和剩余人数应该直接比较：

```cpp
s[i][0] <= n
```

或者统一使用：

```cpp
int take = min(s[i][0], remaining);
```

### 错误 2：`long long` 变量不能自动修复乘法溢出

以下写法仍然危险：

```cpp
long long z;
z += s[i][0] * s[i][1];
```

因为两个乘数都是 `int`，乘法会先按 `int` 计算，然后才加入 `z`。应写成：

```cpp
z += 1LL * s[i][0] * s[i][1];
```

`1LL` 是 `long long` 类型的整数常量。它会让后续乘法转换为 `long long` 运算。

### 反例

```text
1
6 100
1 1 2 1 1 1
1 2 3 4 5 6
```

当处理到下标 `i = 2` 时，剩余人数为 3，而当前居民容量只有 2。使用 `i + a_i` 的旧条件会错误地进入“通知 3 人”的分支，得到非法费用 `112`。合法最优费用是 `113`。

## 复杂度

- 排序：`O(n log n)`。
- 扫描：`O(n)`。
- 总复杂度：`O(n log n)`。
- 额外存储：`O(n)`。

官方约束保证所有测试用例的 `n` 之和不超过 `10^5`，这个复杂度满足要求。[Codeforces 题面](https://codeforces.com/problemset/problem/1876/A?locale=en)

## 使用规则

1. 先明确每个变量当前代表什么。
2. 排序依据必须和费用或目标直接相关，本题按 `b_i` 升序。
3. “最多能处理多少”和“还剩多少”取最小值。
4. 费用可能达到 `10^10` 时使用 `long long`。
5. 看到贪心解法，至少写出一次交换论证或主动构造反例。

## 当前状态

已完成本题的代码纠错和关键知识归纳；仍需独立重写、运行边界样例，并完成另一道不同类型的贪心题，暂不计为“贪心大类已掌握”。

## 关联

C++ 学习笔记 · std::sort：区间、比较器与头文件 · 整数类型范围与选择 · 基础算法训练清单（未收入本公开仓库）

## 来源

- [Codeforces 1876A：Helmets in Night Light](https://codeforces.com/problemset/problem/1876/A?locale=en)：题目、约束和样例。
- [cppreference：整数常量](https://en.cppreference.com/w/cpp/language/integer_literal)：`LL` 整数常量后缀。
- [cppreference：算术运算符](https://en.cppreference.com/cpp/language/operator_arithmetic)：整数提升、乘法和有符号整数溢出。
- [Stanford CS161：Guide to Greedy Algorithms](https://web.stanford.edu/class/archive/cs/cs161/cs161.1138/handouts/120%20Guide%20to%20Greedy%20Algorithms.pdf)：贪心保持领先与正确性证明。
- [Cornell CS482：Exchange Arguments](https://www.cs.cornell.edu/courses/cs482/2007sp/exchange.pdf)：交换论证的基本结构。
