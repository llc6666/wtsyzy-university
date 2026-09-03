---
title: stack 与 queue：LIFO、FIFO 与场景选择
tags:
  - c-plus-plus
  - stl
  - stack
  - queue
  - data-structure
created: 2026-08-27
---

# stack 与 queue：LIFO、FIFO 与场景选择

## 一句话结论

`stack`遵循后进先出（LIFO），`queue`遵循先进先出（FIFO）。判断使用哪一个，先看“下一步应该处理最近加入的元素，还是最早加入的元素”。

## 工作机制

### stack：后进先出

像一摞盘子：最后放上的盘子最先拿走。

```text
push 10，push 20，push 30
取出顺序：30 -> 20 -> 10
```

### queue：先进先出

像排队：先到的人先处理。

```text
push 10，push 20，push 30
取出顺序：10 -> 20 -> 30
```

## 接口对比

| 目的 | `stack` | `queue` |
| --- | --- | --- |
| 放入元素 | `push` | `push` |
| 查看下一元素 | `top` | `front` |
| 删除下一元素 | `pop` | `pop` |
| 查看末端元素 | 无 | `back` |
| 访问中间元素 | 不提供 | 不提供 |

两者的 `pop()`都只负责删除，不返回元素。都应先读取，再删除。

## 最小示例

```cpp
#include <iostream>
#include <queue>
#include <stack>

int main() {
    std::stack<int> st;
    std::queue<int> qu;

    for (int x : {10, 20, 30}) {
        st.push(x);
        qu.push(x);
    }

    while (!st.empty()) {
        std::cout << st.top() << ' '; // 30 20 10
        st.pop();
    }
    std::cout << '\n';

    while (!qu.empty()) {
        std::cout << qu.front() << ' '; // 10 20 30
        qu.pop();
    }
    std::cout << '\n';
}
```

## 什么时候用哪一个

| 问题结构 | 适合的结构 | 原因 |
| --- | --- | --- |
| 括号匹配 | `stack` | 最近打开的括号必须最先匹配 |
| 撤销操作 | `stack` | 最近一次操作最先撤销 |
| 迭代 DFS | `stack` | 先沿最近加入的分支深入 |
| BFS 分层搜索 | `queue` | 先发现的节点先扩展 |
| 消息按到达顺序处理 | `queue` | 保持到达顺序 |
| 滑动窗口逐个淘汰旧数据 | `queue`或`deque` | 从一端加入、另一端移除 |

DFS和BFS的结构选择来自它们的访问顺序。算法题中也可以手写数组模拟，但先确认顺序，再选择容器。

## 与知识库中手写队列的关系

知识库已有的手写链式队列需要自己维护节点、`head`和`tail`。`std::queue`把这些细节封装起来，只暴露先进先出所需的接口。

二者的抽象规则相同，代码责任不同：

- 手写队列适合学习指针、节点和边界。
- `std::queue`适合解题时直接使用稳定的 FIFO 接口。

## 常见错误与边界

### 把 `queue`当成“按大小取最优”

`queue`只按进入顺序处理。需要每次取最大或最小元素时，应考虑 `priority_queue`。

### 空容器读取元素

```cpp
std::queue<int> qu;
// qu.front(); // 错误：必须先判断 qu.empty()
```

### 把 `stack`和`queue`的读取接口写混

```cpp
st.top();   // stack
qu.front(); // queue
```

## 使用规则

1. 最近加入的元素优先处理，用 `stack`。
2. 最早加入的元素优先处理，用 `queue`。
3. 需要按大小取元素，用 `priority_queue`，不要用普通 `queue`。
4. 使用 `top()`或`front()`前先判空。
5. 先保存元素，再调用 `pop()`。

## 关联

stack：核心操作与 vector 模拟栈 · queue 初始化：元素类型、底层容器与初始数据 · priority_queue：三个模板参数与比较器 · 队列：先进先出与双指针 · 手写队列出队逻辑

## 来源

- C++ 工作草案，[stack]：栈容器适配器。<https://eel.is/c++draft/stack>
- C++ 工作草案，[queue]：队列和优先队列容器适配器。<https://eel.is/c++draft/queue>
