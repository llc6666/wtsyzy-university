---
title: STL 常用头文件：功能与包含规则
tags:
  - c-plus-plus
  - stl
  - header
  - include
created: 2026-08-28
---

# STL 常用头文件：功能与包含规则

## 一句话结论

代码直接使用哪个标准库类型或函数，就直接包含声明它的头文件。不要因为某个编译器“碰巧间接包含了”某个头文件，就省略自己的 `#include`。

## 它解决什么问题

头文件可以理解成“声明的入口”。编译器编译当前 `.cpp`文件时，需要先看到 `std::queue`、`std::vector`或`std::greater`等名字的声明，才能检查和编译代码。

例如：

```cpp
#include <queue>

std::queue<int> q;
```

`<queue>`负责提供 `std::queue`和`std::priority_queue`的声明。

## 本批 STL 直接涉及的头文件

| 头文件 | 主要内容 | 当前学习中的典型名字 |
| --- | --- | --- |
| `<iostream>` | 标准输入输出流 | `std::cin`、`std::cout` |
| `<string>` | 动态字符串 | `std::string`、`substr` |
| `<vector>` | 可变长连续序列容器 | `std::vector` |
| `<stack>` | 栈容器适配器 | `std::stack` |
| `<queue>` | 队列和优先队列适配器 | `std::queue`、`std::priority_queue` |
| `<deque>` | 双端队列容器 | `std::deque` |
| `<list>` | 双向链表容器 | `std::list` |
| `<functional>` | 常用函数对象 | `std::less`、`std::greater` |
| `<utility>` | 通用工具、二元组和移动工具 | `std::pair`、`std::move` |
| `<cstddef>` | 标准大小和指针相关类型 | `std::size_t` |

其中 `<deque>`、`<list>`、`<utility>`和`<cstddef>`是否出现，取决于代码是否直接写出对应名字。它们不能因为“某个容器内部可能用到”就省略。

## 按代码选择最小头文件

### 只使用普通 queue

```cpp
#include <queue>

int main() {
    std::queue<int> q;
    q.push(10);
}
```

这里不需要手动包含 `<deque>`，因为代码没有直接写出 `std::deque`；`queue`的默认底层容器由其自身接口处理。

### 明确写出 deque 初始化数据

```cpp
#include <deque>
#include <queue>

int main() {
    std::deque<int> data{10, 20, 30};
    std::queue<int> q(data);
}
```

`<deque>`对应 `std::deque`，`<queue>`对应 `std::queue`。两个名字都在代码中出现，所以两个头文件都应直接包含。

### 使用 list 作为 queue 的底层容器

```cpp
#include <list>
#include <queue>

int main() {
    std::queue<int, std::list<int>> q;
    q.push(10);
}
```

`std::list<int>`必须由 `<list>`提供声明，`std::queue`由 `<queue>`提供声明。

### 使用 priority_queue 的小根堆写法

```cpp
#include <functional>
#include <queue>
#include <vector>

int main() {
    std::priority_queue<
        int,
        std::vector<int>,
        std::greater<int>
    > q;
    q.push(3);
}
```

三个名字对应三个头文件：

```text
std::priority_queue -> <queue>
std::vector         -> <vector>
std::greater        -> <functional>
```

虽然有些实现可能通过 `<queue>`间接带入其他声明，但这种间接关系不应作为代码的依赖。

### 使用 std::move 初始化 queue

```cpp
#include <deque>
#include <queue>
#include <utility>

int main() {
    std::deque<int> data{1, 2, 3};
    std::queue<int> q(std::move(data));
}
```

`std::move`的声明由 `<utility>`提供。这里的移动语义属于另一个知识点，当前只需要记住对应头文件。

### 使用 size_t 遍历 vector

```cpp
#include <cstddef>
#include <vector>

int main() {
    std::vector<int> values{1, 2, 3};
    for (std::size_t i = 0; i < values.size(); ++i) {
    }
}
```

`std::size_t`由 `<cstddef>`提供，`std::vector`由 `<vector>`提供。

## 头文件和类型的对应关系

可以先按下面这张表建立“看到名字就找头文件”的反射：

| 代码中出现的名字 | 直接包含 |
| --- | --- |
| `std::cin`、`std::cout` | `<iostream>` |
| `std::string` | `<string>` |
| `std::vector` | `<vector>` |
| `std::stack` | `<stack>` |
| `std::queue`、`std::priority_queue` | `<queue>` |
| `std::deque` | `<deque>` |
| `std::list` | `<list>` |
| `std::greater`、`std::less` | `<functional>` |
| `std::move`、`std::pair` | `<utility>` |
| `std::size_t` | `<cstddef>` |

## 一段完整示例

```cpp
#include <cstddef>
#include <deque>
#include <functional>
#include <iostream>
#include <list>
#include <queue>
#include <stack>
#include <string>
#include <utility>
#include <vector>

int main() {
    std::string text = "abc";
    std::vector<int> values{1, 2, 3};

    std::stack<int> st;
    std::queue<int> qu;
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;

    std::deque<int> data{10, 20};
    std::queue<int> fromDeque(std::move(data));
    std::queue<int, std::list<int>> fromList;
    fromList.push(30);

    std::pair<int, int> point{1, 2};
    st.push(1);
    qu.push(2);
    minHeap.push(3);

    for (std::size_t i = 0; i < values.size(); ++i) {
        std::cout << values[i] << ' ';
    }
    std::cout << text << ' ' << point.first << ' '
              << st.top() << ' ' << qu.front() << ' '
              << minHeap.top() << ' ' << fromDeque.front() << ' '
              << fromList.front() << '\n';
}
```

这段代码只是展示头文件和名字的对应关系。`std::move(data)`用于把已有 `deque`移动构造成 `queue`；移动后的 `data`不再作为原来的数据源使用。

## 常见错误与边界

### 依赖万能头文件

```cpp
// #include <bits/stdc++.h>
```

`bits/stdc++.h`不是 C++ 标准头文件，常见于部分 GCC 竞赛环境。学习和可移植代码应写标准头文件：

```cpp
#include <iostream>
#include <queue>
#include <vector>
```

### 只包含使用结果的头文件

```cpp
#include <queue>

// 直接写 std::vector<int>，却不包含 <vector>
std::vector<int> values;
```

即使当前编译器能够编译，也不应依赖 `<queue>`的间接包含。代码直接使用 `std::vector`，就直接包含 `<vector>`。

### 把头文件名和类名混淆

```text
头文件：<queue>
类型：std::queue、std::priority_queue
```

尖括号中的名称是文件，`std::`后的名称是类型或函数。它们相关，但不是同一个名字。

### 误以为包含头文件就创建了对象

```cpp
#include <queue>
```

这句只让 `std::queue`这个名字可被识别，不会自动创建任何队列。还需要声明对象：

```cpp
std::queue<int> q;
```

## 使用规则

1. 直接使用哪个名字，就直接包含它对应的标准头文件。
2. `queue`和`priority_queue`都来自 `<queue>`。
3. 小根堆写 `std::greater`时，显式包含 `<functional>`。
4. 优先队列模板参数中写出 `std::vector`时，显式包含 `<vector>`。
5. 只有直接写出 `std::deque`或`std::list`时，才需要在当前文件中显式使用它们的头文件。
6. 需要 `std::move`包含 `<utility>`，需要 `std::size_t`包含 `<cstddef>`。
7. 学习和可移植代码优先使用标准头文件，不依赖 `bits/stdc++.h`和间接包含。

## 关联

queue 初始化 · priority_queue：三个模板参数与比较器 · vector 的 size、size_t 与 static_cast · string 初始化 · stack：核心操作与 vector 模拟栈

## 来源

- C++ 工作草案，[Input/output library]：输入输出库及 `<iostream>`相关设施。<https://eel.is/c++draft/input.output>
- C++ 工作草案，[basic_string]：`std::string`。<https://eel.is/c++draft/basic.string>
- C++ 工作草案，[vector]：`std::vector`。<https://eel.is/c++draft/vector>
- C++ 工作草案，[stack]：`std::stack`。<https://eel.is/c++draft/stack>
- C++ 工作草案，[queue]：`std::queue`和`std::priority_queue`。<https://eel.is/c++draft/queue>
- C++ 工作草案，[deque]：`std::deque`。<https://eel.is/c++draft/deque>
- C++ 工作草案，[list]：`std::list`。<https://eel.is/c++draft/list>
- C++ 工作草案，[comparisons]：`std::less`和`std::greater`。<https://eel.is/c++draft/comparisons>
- C++ 工作草案，[utility]：`std::pair`、`std::move`等工具。<https://eel.is/c++draft/utility>
- C++ 工作草案，[support.types]：标准大小相关类型。<https://eel.is/c++draft/support.types>
