---
title: for 与 do-while：循环的语法和用法
tags:
  - c-plus-plus
  - control-flow
  - loop
created: 2026-08-11
---

# for 与 do-while：循环的语法和用法

## 一句话结论

循环用于重复执行代码。已知“从哪里开始、何时结束、每次如何变化”时，优先使用 `for`；必须先执行一次循环体，再决定是否继续时，使用 `do-while`。

## `for` 循环的固定格式

```cpp
for (初始化; 继续条件; 每轮变化) {
    循环体;
}
```

```cpp
for (int i = 0; i < 5; ++i) {
    std::cout << i << '\n';
}
```

执行顺序：

1. 执行一次初始化：`int i = 0`。
2. 判断条件：`i < 5`。
3. 条件为真，执行循环体。
4. 执行每轮变化：`++i`。
5. 回到第 2 步，条件为假时结束。

上例输出 `0` 到 `4`，不会输出 `5`。原因是 `i == 5` 时，`i < 5` 已为假。

## `for` 的三个部分分别做什么

```cpp
for (int i = 0; i < n; ++i) {
    // 使用 i
}
```

| 部分 | 示例 | 作用 |
| --- | --- | --- |
| 初始化 | `int i = 0` | 只执行一次，建立循环变量 |
| 条件 | `i < n` | 每轮开始前判断，假则停止 |
| 变化 | `++i` | 每轮循环体结束后执行，避免循环变量不变 |

循环变量 `i` 的作用域通常只在这个 `for` 循环中。循环结束后不能再使用它。

## `do-while`：至少执行一次

```cpp
do {
    循环体;
} while (继续条件);
```

注意最后的分号：

```cpp
int number = 0;

do {
    std::cout << "请输入 1 到 10：";
    std::cin >> number;
} while (number < 1 || number > 10);
```

`do-while` 的顺序是：先执行循环体，再判断条件。因此即使条件一开始就是假，循环体也会执行一次。

```cpp
int i = 10;

do {
    std::cout << i << '\n';
} while (i < 5);
```

仍会输出一次 `10`。

## 与 `while` 的核心区别

```cpp
while (i < 5) {
    // 先判断，可能一次都不执行
}
```

```cpp
do {
    // 先执行，至少执行一次
} while (i < 5);
```

输入校验、菜单反复显示等“必须先让用户操作一次”的场景适合 `do-while`。普通计数、遍历数组或已知次数的重复操作更适合 `for`。

## 链表中的 `for` 用法

链表遍历也可使用 `for`：

```cpp
int List::size() const {
    int count = 0;

    for (const Node* p = head; p != nullptr; p = p->next) {
        ++count;
    }

    return count;
}
```

这里三个位置的分工很清楚：

- 初始化：`p = head`
- 条件：`p != nullptr`
- 变化：`p = p->next`

它与 `while` 遍历等价，但把“从哪里开始、何时停止、如何前进”放在同一行，更适合简单遍历。

## 如何提前打断循环

最常用的是 `break`：它立刻结束**最内层**的 `for`、`while`、`do-while` 或 `switch`，程序从该语句之后继续执行。

```cpp
for (int i = 0; i < 10; ++i) {
    if (i == 5) {
        break;
    }
    std::cout << i << ' ';
}
std::cout << "循环结束\n";
```

输出：

```text
0 1 2 3 4 循环结束
```

当找到目标就不必继续查找时，`break` 很合适：

```cpp
bool found = false;

for (int i = 0; i < 5; ++i) {
    if (values[i] == target) {
        found = true;
        break;
    }
}
```

### `break`、`continue` 与 `return` 的区别

| 语句 | 作用范围 | 效果 |
| --- | --- | --- |
| `break` | 最内层循环或 `switch` | 结束这一层循环 |
| `continue` | 最内层循环 | 跳过本轮剩余代码，进入下一轮 |
| `return` | 整个函数 | 直接结束函数，并可交回结果 |

```cpp
for (int i = 0; i < 5; ++i) {
    if (i == 2) {
        continue; // 不输出 2，但循环还会继续
    }
    std::cout << i << ' ';
}
```

在 `find()` 这种“找到就结束函数”的场景，直接 `return` 通常更清楚：

```cpp
int List::find(int target) const {
    int index = 0;
    for (const Node* p = head; p != nullptr; p = p->next, ++index) {
        if (p->data == target) {
            return index;
        }
    }
    return -1;
}
```

### 嵌套循环时的边界

`break` 只能结束它所在的最内层循环：

```cpp
for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
        if (col == 1) {
            break; // 只结束内层 col 循环
        }
    }
}
```

若要在嵌套循环中找到目标后彻底停止，通常用 `return` 退出函数，或用一个布尔变量让外层循环也停止。不要为此滥用 `goto`。

## 常见错误

### 少写变化步骤，造成死循环

```cpp
for (int i = 0; i < 5; ) {
    std::cout << i << '\n';
    // i 没有变化，循环不会结束
}
```

### 误写边界

```cpp
for (int i = 0; i <= 5; ++i) {
    // 执行 6 次：i 为 0、1、2、3、4、5
}
```

若数组有 `n` 个元素，其合法下标是 `0` 到 `n - 1`，通常应写：

```cpp
for (int i = 0; i < n; ++i) {
}
```

### 忘记 `do-while` 结尾的分号

```cpp
do {
    // ...
} while (condition); // 这里必须有 ;
```

## 使用规则

1. 次数明确、需要计数器时，用 `for`。
2. 先执行一次再判断是否继续时，用 `do-while`。
3. 循环条件必须能够最终变为假，否则就是死循环。
4. 数组下标循环通常从 `0` 开始，条件写 `i < 元素个数`。
5. 循环体一律使用花括号。

## 关联

C++ 学习笔记 · i++ 与 ++i：后置和前置自增 · if、else 与条件分支：语法和用法 · List 的 size 与 find：链表遍历指针的使用

## 来源

- C++ 工作草案，[stmt.for]：`for` 的初始化、条件、迭代表达式和等价控制流程。<https://eel.is/c++draft/stmt.for>
- C++ 工作草案，[stmt.do]：`do-while` 在循环体执行后判断条件。<https://eel.is/c++draft/stmt.do>
- C++ 工作草案，[stmt.while]：`while` 在每次执行循环体前判断条件。<https://eel.is/c++draft/stmt.while>
- C++ 工作草案，[stmt.break]：`break` 终止最内层可中断语句。<https://eel.is/c++draft/stmt.break>
- C++ 工作草案，[stmt.cont]：`continue` 进入最内层循环的下一次迭代。<https://eel.is/c++draft/stmt.cont>
