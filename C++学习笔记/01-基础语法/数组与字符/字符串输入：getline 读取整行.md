---
title: 字符串输入：getline 读取整行
tags:
  - c-plus-plus
  - string
  - input
created: 2026-09-03
---

# 字符串输入：getline 读取整行

## 一句话结论

输入可能包含空格的一整行文字时，使用 `std::getline`；`std::cin >> s` 遇到空白通常就停止。

## 头文件

```cpp
#include <iostream>  // 提供 std::cin 和 std::cout
#include <string>    // 提供 std::string 和 std::getline
```

## 基本语法

```cpp
std::string s;                    // 创建用于保存一行文字的字符串
std::getline(std::cin, s);        // 读取一整行，空格也会被保存
std::cout << s << '\n';           // 输出读取到的整行内容
```

## 完整示例

```cpp
#include <iostream>               // 提供标准输入输出
#include <string>                 // 提供字符串类型和 getline

int main() {
    std::string s;                // 准备保存输入的一整行
    std::getline(std::cin, s);    // 从 cin 读取到换行符为止

    std::cout << s << '\n';       // 输出原始字符串
    return 0;                     // 程序正常结束
}
```

输入：

```text
#Poltu is new here
```

输出：

```text
#Poltu is new here
```

## 与 `cin >> s` 的区别

```cpp
std::cin >> s;                    // 读取一个以空白分隔的单词
```

输入：

```text
#Poltu is new here
```

通常只读取：

```text
#Poltu
```

而：

```cpp
std::getline(std::cin, s);        // 读取整行，直到换行符
```

会读取：

```text
#Poltu is new here
```

## 数字输入后使用 getline

如果前面使用 `std::cin >>` 读取数字，输入流中可能还留着换行符。下一次直接使用 `getline` 可能读到空行：

```cpp
#include <iostream>               // 提供输入输出
#include <string>                 // 提供 string 和 getline
#include <limits>                 // 提供 numeric_limits

int main() {
    int n;                        // 保存一个整数
    std::string s;                // 保存后续的一整行

    std::cin >> n;                // 先读取整数，换行符可能仍在输入流中
    std::cin.ignore(              // 丢弃本行剩余字符，避免 getline 立即读到空行
        std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, s);    // 再读取真正的一整行

    std::cout << n << '\n';       // 输出整数
    std::cout << s << '\n';       // 输出整行字符串
    return 0;                     // 程序正常结束
}
```

## C++98 兼容性

`std::string` 和 `std::getline` 在 C++98 中已经存在。下面这种范围 `for` 才是 C++11 语法：

```cpp
for (char ch : s) { }
```

旧版编译器可以使用普通下标循环，见 字符串去空格：逐字符筛选与结果构造。

## 常见误区

1. 需要整行时使用 `cin >> s`，会丢失第一个空格之后的内容。
2. 数字输入后直接 `getline`，可能先读到残留换行符。
3. 把 `getline` 当作删除空格的函数；它只负责读取，不负责修改字符串。

## 如何验证

先输入整数 `42` 并回车，再调用 `getline` 读取 `hello world`。若没有先忽略遗留换行，第一次读取会得到空字符串；按文中的 `std::cin.ignore(...)` 处理后，输出应为完整的 `hello world`。再对比 `cin >> s`，它只会读到 `hello`。

## 边界

`getline` 默认以换行结束，不会自动跳过前序格式化输入留下的换行符。混用 `>>` 和 `getline` 时，每一处转换都要明确处理缓冲区。

## 关联

字符串去空格：逐字符筛选与结果构造 · 字符与字符串输入：char 数组、cin 与 strlen
