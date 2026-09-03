---
title: stack：核心操作与 vector 模拟栈
tags:
  - c-plus-plus
  - stl
  - stack
  - data-structure
created: 2026-08-27
---

# stack：核心操作与 vector 模拟栈

## 一句话结论

`std::stack`只允许从一端操作，遵循后进先出（LIFO）：最后压入的元素最先弹出。取值要先 `top()`，再 `pop()`；`pop()`本身不返回被删除的元素。

## 它解决什么问题

当问题只需要“查看并删除最近加入的元素”时，栈能限制操作范围，避免代码随意访问中间元素。

典型场景包括括号匹配、撤销操作、递归过程的显式模拟和迭代 DFS。

## 工作机制

```cpp
std::stack<int> st;
```

默认情况下，`std::stack`以一个底层序列容器保存元素，并只暴露栈需要的接口。标准允许使用支持 `back()`、`push_back()`和`pop_back()`的序列容器实例化它；默认底层容器是 `std::deque<T>`。

常用接口：

| 操作 | 写法 | 作用 |
| --- | --- | --- |
| 压栈 | `st.push(x)` | 把 x 放到栈顶 |
| 取栈顶 | `st.top()` | 访问栈顶，不删除 |
| 出栈 | `st.pop()` | 删除栈顶，不返回值 |
| 判空 | `st.empty()` | 判断是否没有元素 |
| 获取大小 | `st.size()` | 获取元素数量 |

## 最小示例

```cpp
#include <iostream>
#include <stack>

int main() {
    std::stack<int> st;
    st.push(10);
    st.push(20);
    st.push(30);

    while (!st.empty()) {
        std::cout << st.top() << ' '; // 30 20 10
        st.pop();
    }
    std::cout << '\n';
}
```

正确的“取出一个元素”流程：

```cpp
int value = st.top();
st.pop();
```

## `vector`也能模拟栈

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> st;
    st.push_back(10); // 入栈
    st.push_back(20);

    std::cout << st.back() << '\n'; // 栈顶 20
    st.pop_back();                  // 出栈
}
```

对应关系是：

| 栈操作 | `vector`操作 |
| --- | --- |
| `push` | `push_back` |
| `top` | `back` |
| `pop` | `pop_back` |
| `empty` | `empty` |
| `size` | `size` |

## 为什么还要使用 `stack`

`vector`提供下标访问、迭代和更多修改能力；`stack`只暴露栈接口。使用 `stack`时，代码结构会直接表达“只能从栈顶操作”的约束。

如果确实需要访问中间元素或排序，使用 `vector`更合适；如果只需要 LIFO 行为，使用 `stack`更清楚。

## 常见错误与边界

### 空栈取 `top()`

```cpp
std::stack<int> st;
// st.top(); // 错误：必须先确认 !st.empty()
```

### 误以为 `pop()`返回元素

```cpp
// int value = st.pop(); // 错误
int value = st.top();
st.pop();
```

### 试图使用下标或范围 `for`

```cpp
// st[0];                 // 错误：stack没有下标接口
// for (auto x : st) {}   // 错误：stack不提供迭代接口
```

## 使用规则

1. 使用前检查 `empty()`。
2. 取值和删除分两步：先 `top()`，后 `pop()`。
3. 只需要 LIFO 时用 `stack`；需要随机访问时用 `vector`。
4. 不要把 `stack`当作可遍历数组。
5. 迭代 DFS 可以使用 `stack`，递归 DFS则使用调用栈。

## 关联

stack 与 queue：LIFO、FIFO 与场景选择 · priority_queue：三个模板参数与比较器 · 队列：先进先出与双指针

## 来源

- C++ 工作草案，[stack]：容器适配器、默认底层容器和核心接口。<https://eel.is/c++draft/stack>
- 本地参考资料：Haotian-BiJi STL 原始资料
