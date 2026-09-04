---
title: queue 初始化：元素类型、底层容器与初始数据
tags:
  - c-plus-plus
  - stl
  - queue
  - initialization
  - data-structure
created: 2026-08-28
---

# queue 初始化：元素类型、底层容器与初始数据

## 一句话结论

最常用的 `queue` 初始化是：

```cpp
std::queue<int> q;
```

它创建一个存放 `int` 的空队列。之后通过 `push` 放入元素，不能像 `vector` 一样靠 `queue<int> q(3)`创建 3 个元素，也不能用下标访问队列内部。

## 它解决什么问题

`std::queue`是一个容器适配器。它把底层容器包装起来，只开放先进先出所需的操作：

```text
队首：front()  ->  [10] [20] [30]  <-  队尾：back()
                         push 从队尾加入
                         pop 从队首删除
```

初始化时要分清两件事：

1. 队列保存什么类型的元素。
2. 队列内部使用什么容器保存这些元素。

## 工作机制

完整的模板形式是：

```cpp
std::queue<T, Container> q;
```

| 位置 | 含义 | 示例 |
| --- | --- | --- |
| `T` | 队列元素类型 | `int` |
| `Container` | 底层容器类型 | `std::deque<int>` |

最常用写法省略第二个参数：

```cpp
std::queue<int> q;
```

它等价于：

```cpp
std::queue<int, std::deque<int>> q;
```

这里的“等价”指模板参数的默认值相同。`queue`只要求底层容器支持 `front()`、`back()`、`push_back()`和`pop_front()`；标准中 `deque`和`list`都满足这些要求。

## 四种常见初始化方式

### 1. 默认初始化：空队列

```cpp
std::queue<int> q;
std::queue<int> other{};
```

两者都创建空队列：

```cpp
q.push(10);
q.push(20);
```

此时队列顺序是 `10 -> 20`，队首是 10。

### 2. 指定底层容器类型

```cpp
#include <list>
#include <queue>

std::queue<int, std::list<int>> q;
```

这里第一个 `int`是元素类型，第二个 `std::list<int>`是底层容器类型。底层容器的元素类型必须与 `T`匹配。

初学和算法竞赛中通常直接使用默认的 `deque`，不需要手动指定第二个参数。

### 3. 用已有的 `deque`初始化

```cpp
#include <deque>
#include <queue>
#include <utility>

std::deque<int> data{10, 20, 30};
std::queue<int> q(data); // 复制 data 中的元素
```

初始化后，队首是 10，接下来依次是 20、30。

也可以移动已有容器的内容：

```cpp
std::queue<int> q(std::move(data));
```

移动后不要再把 `data`当作原来仍然装有这三个元素的队列底层数据使用。

### 4. 先初始化空队列，再逐个加入

这是最适合初学和做题的方式：

```cpp
std::queue<int> q;
q.push(10);
q.push(20);
q.push(30);
```

`push`改变队列中的元素数量；初始化声明只负责创建队列对象。

## 最小示例

```cpp
#include <iostream>
#include <queue>

int main() {
    std::queue<int> q;

    std::cout << q.empty() << '\n'; // 1，刚创建时为空

    q.push(10);
    q.push(20);
    q.push(30);

    std::cout << q.front() << '\n'; // 10，队首
    std::cout << q.back() << '\n';  // 30，队尾

    while (!q.empty()) {
        std::cout << q.front() << ' ';
        q.pop();
    }
    std::cout << '\n'; // 10 20 30
}
```

## 为什么 `queue`不像 `vector`那样初始化

`vector`本身就是顺序容器，提供了“数量、初值”构造：

```cpp
std::vector<int> v(3, 10); // 3个10
```

`queue`是容器适配器，核心任务是限制访问方式。它的常用构造参数主要是底层容器，而不是“元素数量、初值”：

```cpp
std::deque<int> data(3, 10);
std::queue<int> q(data); // 先创建deque，再用它构造queue
```

因此，下面的直觉不成立：

```cpp
// std::queue<int> q(3);       // 不能理解为“3个int元素”
// std::queue<int> q(3, 10);   // 不能理解为“3个10”
```

如果需要批量准备初始数据，先构造 `deque`，再交给 `queue`；如果只是解题，直接空队列配合 `push`更清楚。

## `queue<int> q{1, 2, 3}`的边界

不要把 `queue`的花括号初始化直接类比为 `vector`：

```cpp
// std::queue<int> q{1, 2, 3};
```

在常用的 C++17 接口中，`queue`没有像 `vector`那样的 `initializer_list`构造函数，这种写法不能作为通用写法依赖。可移植的 C++17 写法是：

```cpp
std::queue<int> q(std::deque<int>{1, 2, 3});
```

C++23 增加了基于范围构造和 `push_range`等能力，但这不改变初学阶段最重要的模型：`queue`通过 `push`进入，通过 `front`读取，通过 `pop`删除。

## 常见错误与边界

### 把第二个模板参数当成第二种元素类型

```cpp
std::queue<int, std::deque<int>> q; // 正确：int和deque<int>是一组匹配关系
// std::queue<int, double> q;        // 错误：double不是合适的底层容器
```

### 试图用下标读取

```cpp
// q[0]; // 错误：queue没有下标接口
```

只允许从队首读取：

```cpp
if (!q.empty()) {
    int first = q.front();
    q.pop();
}
```

### 误以为初始化一个空队列会自动放入默认元素

```cpp
std::queue<int> q;
// q.front(); // 错误：q中没有元素
```

`int`的默认值不会凭空变成一个队列元素。空队列没有队首，必须先 `push`。

### 把已有底层容器和已有 queue 混用

```cpp
std::deque<int> data{1, 2, 3};
std::queue<int> q(data);
```

`q`是一个新的 `queue`对象，默认构造形式会复制 `data`中的元素。之后对 `q`调用 `push`或`pop`，不会同步修改原来的 `data`。

## 如何验证

用 `deque<int>{1, 2, 3}` 初始化 `queue<int>` 后，检查 `size()` 为 3、`front()` 为 1、`back()` 为 3。再执行一次 `pop()`，前端应变为 2。尝试 `queue<int> q(3)` 或 `q{1, 2, 3}`，应观察到编译失败，不能把 queue 当作 vector 初始化。

## 使用规则

1. `std::queue<T> q;`表示创建存放 `T`的空队列。
2. 默认底层容器是 `std::deque<T>`。
3. 第二个模板参数是底层容器类型，不是第二个元素类型。
4. 初始化后用 `push`加入元素，用 `front`读取，用 `pop`删除。
5. `queue`没有 `vector`式的数量、初值构造，也没有下标访问。
6. 需要预置多个元素时，可先构造 `deque`或`list`，再构造 `queue`。
7. 使用 `front()`、`back()`和`pop()`前先判断 `empty()`。

## 关联

STL 常用头文件：功能与包含规则 · stack 与 queue：LIFO、FIFO 与场景选择 · vector 初始化：大小、初值与列表初始化 · 队列：先进先出与双指针 · 手写队列出队逻辑

## 来源

- C++ 工作草案，[queue]：`queue<T, Container>`的默认底层容器、构造函数和核心接口。<https://eel.is/c++draft/queue>
- C++ 工作草案，[deque]：`deque`的构造函数和首尾操作。<https://eel.is/c++draft/deque>
- 本地参考资料：Haotian-BiJi STL 原始资料
