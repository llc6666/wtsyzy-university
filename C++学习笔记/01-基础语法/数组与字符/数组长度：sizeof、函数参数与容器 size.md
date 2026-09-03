---
title: 数组长度：sizeof、函数参数与容器 size
tags:
  - c-plus-plus
  - array
  - size
created: 2026-09-03
---

# 数组长度：sizeof、函数参数与容器 size

## 一句话结论

原生数组在定义它的作用域内可以用 `sizeof(数组) / sizeof(数组[0])` 求元素个数；传入函数后通常退化为指针，函数内部不能再用这个方法得到原数组长度，应额外传长度或使用 `std::array`、`std::vector`。

## 原生数组在当前作用域求长度

```cpp
#include <iostream>

int main() {
    int a[5] = {10, 20, 30, 40, 50};       // 定义含有 5 个元素的数组

    int length = sizeof(a) / sizeof(a[0]); // 总字节数除以单个元素字节数

    std::cout << length << '\n';           // 输出数组元素个数：5
    return 0;                              // 程序正常结束
}
```

`sizeof(a)` 得到整个数组占用的字节数，`sizeof(a[0])` 得到一个元素占用的字节数。两者相除才是元素数量。`sizeof` 不会对数组操作数执行数组到指针的转换，因此在数组仍是数组的地方可以得到完整大小。

## 函数中不能直接这样求

```cpp
void print(int a[]) {
    int length = sizeof(a) / sizeof(a[0]); // 错误：a 在这里实际按指针处理
}
```

函数参数中的 `int a[]` 通常等价于 `int* a`。调用函数时，数组会转换为指向首元素的指针，长度信息不会自动传入。

正确做法是把长度作为参数传入：

```cpp
#include <iostream>

void print(const int a[], int length) {   // length 明确表示有效元素个数
    for (int i = 0; i < length; ++i) {     // 只访问合法下标 0 到 length-1
        std::cout << a[i] << ' ';         // 输出当前元素
    }
}

int main() {
    int a[5] = {10, 20, 30, 40, 50};      // 定义原生数组
    int length = sizeof(a) / sizeof(a[0]); // 在调用处求出数组长度

    print(a, length);                     // 同时传入首地址和元素个数
    return 0;                             // 程序正常结束
}
```

## 数组最大长度

C++ 没有一个对所有平台都相同的“数组最大长度”。最大长度同时受元素大小、对象大小限制、内存位置、编译器和运行环境影响。

### 原生数组

```cpp
int a[n];
```

要求 `n` 在编译时可确定，并且整个数组对象必须能够放进当前存储区域。理论上，`int` 数组最多受对象大小和地址空间限制；实际可用长度通常远小于理论值。

如果数组定义在函数内部，它通常位于栈上：

```cpp
void f() {
    int a[10000000]; // 可能导致栈空间不足
}
```

大数组更适合使用 `std::vector` 或动态存储，但动态存储仍受可用内存限制。

### `std::vector`

```cpp
#include <vector>

std::vector<int> a;
std::size_t limit = a.max_size(); // 容器理论上允许的最大元素数量
```

`max_size()` 是实现和类型决定的理论上限，运行时真正能分配的数量还会受 RAM、地址空间和连续内存影响。

### `std::array`

```cpp
#include <array>

std::array<int, 100> a; // 长度在类型中固定为 100
```

`std::array<T, N>` 的最大长度就是模板参数 `N`；它不能在运行时改变长度。

## 实际选择

| 情况 | 建议 |
| --- | --- |
| 长度很小且编译时已知 | 原生数组或 `std::array` |
| 长度运行时才知道 | `std::vector` |
| 数据量很大 | 优先动态容器，并检查内存 |
| 函数局部数组很大 | 避免放在栈上，考虑 `vector` 或静态/动态存储 |

例如，假设 `int` 占 4 字节：

```text
int a[1000]       约 4 KB
int a[1000000]    约 4 MB
int a[100000000]  约 400 MB
```

这只是数据本身的粗略大小，没有计算容器、运行库、栈限制和其他程序内存。

## 不要把理论上限当作可申请大小

下面的计算只能得到“按地址空间和元素大小推算的理论数量”，不能保证申请成功：

```cpp
#include <cstddef>
#include <iostream>
#include <limits>

int main() {
    std::size_t theoretical =   // 估算理论上的 int 元素数量
        std::numeric_limits<std::size_t>::max() / sizeof(int);

    std::cout << theoretical << '\n'; // 结果不代表实际可用长度
}
```

实际编程时，应根据题目数据范围、内存限制和元素类型计算：

```text
所需内存 ≈ 元素个数 × sizeof(元素类型)
```

## 二维数组长度

```cpp
#include <iostream>

int main() {
    int table[2][3] = {{1, 2, 3}, {4, 5, 6}}; // 定义 2 行 3 列数组

    int rows = sizeof(table) / sizeof(table[0]);       // 求行数
    int cols = sizeof(table[0]) / sizeof(table[0][0]); // 求列数

    std::cout << rows << ' ' << cols << '\n';         // 输出：2 3
    return 0;                                          // 程序正常结束
}
```

二维数组按“数组的数组”理解：`table[0]` 是一整行。因此：

```cpp
行数 = sizeof(table) / sizeof(table[0]);
列数 = sizeof(table[0]) / sizeof(table[0][0]);
```

## `std::array` 的长度

```cpp
#include <array>
#include <iostream>

int main() {
    std::array<int, 5> a = {{10, 20, 30, 40, 50}}; // 固定长度容器

    std::cout << a.size() << '\n';                  // 通过成员函数得到元素个数
    return 0;                                        // 程序正常结束
}
```

`std::array` 推荐用于长度固定、需要传递数组对象的场景；长度属于类型的一部分，`a.size()` 表达的就是元素个数。

## `std::vector` 的长度

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> a;                  // 创建可变长度容器
    a.push_back(10);                     // 添加一个元素
    a.push_back(20);                     // 再添加一个元素

    std::cout << a.size() << '\n';       // 输出当前元素个数：2
    return 0;                            // 程序正常结束
}
```

`vector` 使用：

```cpp
a.size()
```

不要使用 `sizeof(a) / sizeof(a[0])` 计算 `vector` 的元素个数，因为 `sizeof(vector)` 得到的是容器对象本身的大小，不是元素数量。

## C++17 的 `std::size`

```cpp
#include <iterator>
#include <iostream>

int main() {
    int a[5] = {1, 2, 3, 4, 5};          // 定义原生数组
    std::cout << std::size(a) << '\n';  // 获取数组元素个数：5
    return 0;                            // 程序正常结束
}
```

`std::size` 可以直接处理原生数组，也可以处理具有 `size()` 成员函数的容器；它从 C++17 开始提供。

旧版 Dev-C++ 不支持 C++17 时，使用 `sizeof` 公式或显式传入长度。

## 常见误区

1. 在函数参数中用 `sizeof(a) / sizeof(a[0])`，此时 `a` 通常已经是指针。
2. 把 `sizeof(vector)` 当作 `vector` 元素数量。
3. 把字符数组的容量当成字符串长度；以 `\0` 结尾的 C 字符串应使用 `strlen` 求有效字符数。
4. 忘记数组下标从 `0` 到 `length - 1`，访问 `a[length]` 会越界。
5. 二维数组求列数时，不能用整个二维数组的总元素数代替每行列数。

## 关联

数组：基本定义与语法 · 字符与字符串输入：char 数组、cin 与 strlen · vector 的 size、size_t 与 static_cast

## 来源

- C++ 标准库 `sizeof` 运算符
- C++ 标准库 `std::size`
- cppreference：数组到指针转换与函数参数调整
