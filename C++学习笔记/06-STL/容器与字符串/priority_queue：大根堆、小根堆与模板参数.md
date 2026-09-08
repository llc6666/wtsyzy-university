---
title: priority_queue：大根堆、小根堆与模板参数
tags:
  - c-plus-plus
  - stl
  - priority-queue
  - heap
created: 2026-09-08
status: 已验证示例
---

# priority_queue：大根堆、小根堆与模板参数

## 适用场景

当程序需要不断加入元素，并且每次优先取出当前最大值或最小值时，可以考虑 `std::priority_queue`。它适合动态取极值，不等于一个可以任意下标访问的完整排序数组。

## 你需要先看懂的声明

```cpp
std::priority_queue<T, Container, Compare> pq;
```

三个模板参数依次表示：

| 位置 | 含义 | 常见写法 |
| --- | --- | --- |
| `T` | 元素类型 | `int`、`double` 或自定义类型 |
| `Container` | 底层存储容器 | `std::vector<T>` |
| `Compare` | 优先级比较规则 | `std::less<T>` 或 `std::greater<T>` |

第二个参数是底层容器，不是“第二种元素类型”。多数情况下可以使用默认容器，因此大根堆只需写：

```cpp
std::priority_queue<int> max_heap;
```

## 大根堆和小根堆

对常见数值类型，最终效果如下：

| 声明 | `top()` 返回 |
| --- | --- |
| `std::priority_queue<int>` | 当前最大值 |
| `std::priority_queue<int, std::vector<int>, std::less<int>>` | 当前最大值 |
| `std::priority_queue<int, std::vector<int>, std::greater<int>>` | 当前最小值 |

小根堆必须把比较器放在第三个模板参数位置：

```cpp
#include <functional>
#include <queue>
#include <vector>

std::priority_queue<
    int,
    std::vector<int>,
    std::greater<int>
> min_heap;
```

不要只凭 `greater` 这个单词猜结果，直接验证 `top()` 的语义更可靠。

## 核心操作

| 操作 | 写法 | 常见复杂度 |
| --- | --- | ---: |
| 插入 | `pq.push(x)` | `O(log n)` |
| 查看堆顶 | `pq.top()` | `O(1)` |
| 删除堆顶 | `pq.pop()` | `O(log n)` |
| 判空 | `pq.empty()` | `O(1)` |
| 查看元素数量 | `pq.size()` | `O(1)` |

`pop()` 只删除，不返回元素。需要保存被删除的值时，先读取再删除：

```cpp
if (!pq.empty()) {
    int best = pq.top();
    pq.pop();
}
```

## 最小可验证示例

```cpp
#include <functional>
#include <iostream>
#include <queue>
#include <vector>

int main() {
    std::priority_queue<int> max_heap;
    std::priority_queue<int, std::vector<int>, std::greater<int>> min_heap;

    for (int value : {5, 1, 8, 3}) {
        max_heap.push(value);
        min_heap.push(value);
    }

    std::cout << max_heap.top() << '\n'; // 8
    std::cout << min_heap.top() << '\n'; // 1
}
```

编译运行后应输出：

```text
8
1
```

## 常见错误与修正

### 把堆当成完整排序结果

```cpp
// pq[0]; // 错误：priority_queue 没有下标接口
```

堆只保证 `top()` 是当前优先级最高的元素，内部其他元素的排列不能当作完整升序或降序序列读取。需要依次取出时，循环执行 `top()` 和 `pop()`，但这会消耗容器。

### 把普通队列和优先队列混用

`std::queue` 按进入顺序取元素；`std::priority_queue` 按优先级取元素。需要保留 FIFO 顺序时，不能用优先队列替代普通队列。

### 把整数构造参数理解成容量

```cpp
// std::priority_queue<int> pq(3); // 不能按 vector(3) 理解
```

优先队列中的元素数量由 `push()` 和 `pop()` 改变。若要限制保留数量，应在插入后主动检查 `size()`。

### 空堆调用 `top()` 或 `pop()`

调用前先检查 `empty()`。这两个操作都要求容器中存在堆顶元素。

## 如何验收

执行配套示例或将四个值 `5、1、8、3` 分别压入大根堆和小根堆，检查：

1. 大根堆的 `top()` 是 `8`。
2. 小根堆的 `top()` 是 `1`。
3. 每次读取后调用 `pop()`，不会得到插入顺序 `5、1、8、3`。
4. 清空后再调用 `top()` 或 `pop()` 属于错误用法。

## 什么时候不适用

- 需要按下标访问、修改或遍历任意元素时，优先队列接口不够用。
- 需要完整排序结果时，应考虑 `std::sort` 或其他排序方法。
- 只需要保持先进先出时，使用 `std::queue`。
- 比较器无法形成稳定的严格弱序时，不应直接用于标准库容器适配器。

## 来源与复核要求

- [C++ 工作草案：`priority_queue`](https://eel.is/c++draft/queue)：模板参数、底层容器和成员函数。
- [C++ 工作草案：比较函数对象](https://eel.is/c++draft/comparisons)：`std::less` 与 `std::greater`。
- [cppreference：`std::priority_queue`](https://en.cppreference.com/w/cpp/container/priority_queue)：接口、复杂度和约束说明。

## 相关页面

- [priority_queue：用小根堆维护前 k 大](priority_queue：用小根堆维护前%20k%20大.md)
- [stack 与 queue：LIFO、FIFO 与场景选择](stack%20与%20queue：LIFO、FIFO%20与场景选择.md)
- [C++ 学习笔记入口](../../README.md)
