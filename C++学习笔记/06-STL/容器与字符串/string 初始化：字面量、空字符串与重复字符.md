---
title: string 初始化：字面量、空字符串与重复字符
tags:
  - c-plus-plus
  - stl
  - string
  - initialization
created: 2026-08-27
---

# string 初始化：字面量、空字符串与重复字符

## 一句话结论

`std::string s = "ava"`和`std::string s("ava")`都会创建一个内容为 `ava`的 `std::string`对象；前者使用拷贝初始化语法，后者使用直接初始化语法。

`"ava"`本身是字符串字面量，`s`才是可以修改内容的 `std::string`对象。

## 它解决什么问题

初学时容易把以下三件事混成一个东西：

1. 字符 `'a'`
2. 字符串字面量 `"ava"`
3. `std::string`对象 `s`

它们的类型、存储方式和可修改性不同。

## 工作机制

字符串字面量表示一段固定的字符序列。它可以用于构造 `std::string`：

```cpp
std::string a = "ava";
std::string b("ava");
```

两句完成后的对象内容相同，但初始化语法不同。

常见初始化对比如下：

| 写法 | 含义 | 结果 |
| --- | --- | --- |
| `std::string s;` | 默认构造 | 空字符串 |
| `std::string s = "ava";` | 从字面量构造 | `ava` |
| `std::string s("ava");` | 直接从字面量构造 | `ava` |
| `std::string s(5, 'a');` | 构造 5 个字符 | `aaaaa` |
| `std::string s{'a', 'b'};` | 用字符列表初始化 | `ab` |

## 最小示例

```cpp
#include <iostream>
#include <string>

int main() {
    std::string empty;
    std::string copyInit = "ava";
    std::string directInit("ava");
    std::string repeated(5, 'a');

    copyInit[0] = 'A';

    std::cout << empty << '\n';       // 空行
    std::cout << copyInit << '\n';    // Ava
    std::cout << directInit << '\n';  // ava
    std::cout << repeated << '\n';    // aaaaa
}
```

## 字符、字面量和字符串对象

```cpp
char ch = 'a';
const char* literal = "ava";
std::string text = "ava";
```

- `ch`只保存一个字符。
- `literal`指向字符串字面量，不能通过它修改字面量内容。
- `text`是一个独立的 `std::string`对象，可以通过下标修改已有字符。

```cpp
text[0] = 'A'; // 合法
// literal[0] = 'A'; // 错误意图：不能修改字符串字面量
```

## 常见错误与边界

### 单引号和双引号混用

```cpp
char a = 'a';          // 一个字符
std::string b = "a";  // 一个字符组成的字符串
```

`'a'`和`"a"`写法只差一个符号，类型却不同。

### `std::string s(5, 'a')`不是“从位置 5 开始的字符串”

它调用的是“数量、字符”构造形式，结果是 5 个 `a`。

### 不能通过越界下标自动扩容

```cpp
std::string s = "abc";
// s[3] = 'd'; // 越界，不能这样追加
s += 'd';
```

## 使用规则

1. 字符用单引号，字符串用双引号。
2. 需要可修改、可增长的文本时使用 `std::string`。
3. `s = "text"`和`s("text")`结果相同，语法机制不同。
4. `std::string(n, ch)`表示重复 n 次字符 ch。
5. 追加内容用 `+=`或`push_back`，不要用越界下标。

## 关联

string 拼接：operator+ 与 += 的性能边界 · string 的 substr：起始位置与长度 · 字符与字符串输入

## 来源

- C++ 工作草案，[basic_string]：字符串对象、连续存储和字符串接口。<https://eel.is/c++draft/basic.string>
- C++ 工作草案，[lex.string]：字符串字面量。<https://eel.is/c++draft/lex.string>
- 本地参考资料：Haotian-BiJi STL 原始资料
