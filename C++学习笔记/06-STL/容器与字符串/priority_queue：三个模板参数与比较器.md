---
title: priority_queue：三个模板参数与比较器
tags:
  - c-plus-plus
  - stl
  - priority-queue
  - heap
created: 2026-08-27
---

# priority_queue：三个模板参数与比较器

## 一句话结论

```cpp
std::priority_queue<T, Container, Compare> pq;
```

三个模板参数依次表示：元素类型、底层容器类型、比较器类型。最常用的默认大根堆是 `std::priority_queue<int>`；小根堆写成 `std::priority_queue<int, std::vector<int>, std::greater<int>>`。

## 它解决什么问题

普通 `queue`按进入顺序取元素。`priority_queue`关心的是优先级：每次都能快速查看当前优先级最高的元素。

例如：

- 每次取当前最大值：默认大根堆。
- 每次取当前最小值：使用 `std::greater<T>`。
- 动态加入元素，同时持续取最大或最小值：使用优先队列。

## 三个模板参数分别是谁

以这句为例：

```cpp
std::priority_queue<int, std::vector<int>, std::greater<int>> pq;
```

| 位置 | 参数 | 含义 |
| --- | --- | --- |
| 第一个 | `int` | 每个元素的类型 |
| 第二个 | `std::vector<int>` | 底层保存元素的容器 |
| 第三个 | `std::greater<int>` | 决定优先级顺序的比较器 |

把它读成一句话：

> 用 `vector<int>`保存`int`，并按 `greater<int>`规定的顺序维护优先队列。

第二个参数不是“另一个元素”，而是优先队列内部使用的存储容器。通常保持默认的 `vector<T>`即可。

## `less`和`greater`为什么对应大小根堆

标准库中的比较器本身表示普通关系：

```cpp
std::less<int>{}(a, b)    // 等价于 a < b
std::greater<int>{}(a, b) // 等价于 a > b
```

优先队列会依据这个关系组织堆顶。对常见数值类型，可以直接记住最终效果：

| 声明 | `top()` |
| --- | --- |
| `std::priority_queue<int>` | 最大值 |
| `std::priority_queue<int, std::vector<int>, std::less<int>>` | 最大值 |
| `std::priority_queue<int, std::vector<int>, std::greater<int>>` | 最小值 |

不要把 `greater`机械理解成“greater更大所以取大”，它的比较方向参与的是堆的组织规则；真正要检查的是 `top()`最终返回最大值还是最小值。

## 最小示例

```cpp
#include <functional>
#include <iostream>
#include <queue>
#include <vector>

int main() {
    std::priority_queue<int> maxHeap;
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;

    for (int x : {5, 1, 8, 3}) {
        maxHeap.push(x);
        minHeap.push(x);
    }

    std::cout << maxHeap.top() << '\n'; // 8
    std::cout << minHeap.top() << '\n'; // 1
}
```

## 核心操作

| 操作 | 写法 | 常见复杂度 |
| --- | --- | ---: |
| 插入 | `pq.push(x)` | `O(log n)` |
| 查看堆顶 | `pq.top()` | `O(1)` |
| 删除堆顶 | `pq.pop()` | `O(log n)` |
| 判空 | `pq.empty()` | `O(1)` |
| 大小 | `pq.size()` | `O(1)` |

与 `stack`、`queue`一样，`pop()`只删除，不返回元素：

```cpp
int best = pq.top();
pq.pop();
```

## 常见错误与边界

### 把 `priority_queue`当成普通队列

```cpp
std::priority_queue<int> pq;
pq.push(2);
pq.push(10);
pq.push(5);
// 取出顺序是 10、5、2，不是 2、10、5
```

### 忘记小根堆的完整写法

```cpp
// std::priority_queue<int, std::greater<int>> pq; // 参数位置错误
std::priority_queue<int, std::vector<int>, std::greater<int>> pq;
```

因为第二个位置要求底层容器，比较器位于第三个位置。

### 试图访问内部任意元素

```cpp
// pq[0]; // 错误：没有下标接口
```

优先队列只保证通过 `top()`访问当前堆顶。其他元素的内部排列服务于堆结构，不能当作完整有序数组读取。

### 空堆读取 `top()`

```cpp
if (!pq.empty()) {
    int x = pq.top();
    pq.pop();
}
```

## 使用规则

1. 先明确每次要取最大值还是最小值。
2. 默认 `priority_queue<T>`是大根堆。
3. 小根堆使用 `std::greater<T>`，并把它放在第三个模板参数位置。
4. 第二个参数通常写 `std::vector<T>`，它表示底层存储容器。
5. `top()`只读堆顶，`pop()`只删除堆顶，不要把它当作可遍历数组。
6. 自定义比较器要满足严格弱序；暂时先掌握默认比较器和 `less/greater`。

## 关联

stack 与 queue：LIFO、FIFO 与场景选择 · stack：核心操作与 vector 模拟栈 · 基础算法训练清单（未收入本公开仓库）

## 来源

- C++ 工作草案，[queue]：`priority_queue`的模板参数、底层容器和接口。<https://eel.is/c++draft/queue>
- C++ 工作草案，[comparisons]：`std::less`和`std::greater`的比较语义。<https://eel.is/c++draft/comparisons>
- 本地参考资料：Haotian-BiJi STL 原始资料
