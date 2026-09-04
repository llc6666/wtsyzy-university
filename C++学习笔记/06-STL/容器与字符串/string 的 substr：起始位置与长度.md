---
title: string 的 substr：起始位置与长度
tags:
  - c-plus-plus
  - stl
  - string
  - substring
created: 2026-08-27
---

# string 的 substr：起始位置与长度

## 一句话结论

`s.substr(pos, count)`的第一个参数是起始下标，第二个参数是截取长度，不是结束下标。

## 它解决什么问题

```cpp
std::string s = "abcdef";
std::string part = s.substr(2, 3);
```

从下标 2开始取 3个字符，因此结果是 `cde`，访问的下标为 2、3、4。

## 工作机制

字符串下标从 0开始：

```text
字符:  a  b  c  d  e  f
下标:  0  1  2  3  4  5
```

调用形式：

```cpp
s.substr(起始下标, 字符数量)
```

第二个参数是 `count`。如果要求的长度超过字符串剩余长度，函数会截取到字符串末尾。

## 最小示例

```cpp
#include <iostream>
#include <string>

int main() {
    std::string s = "abcdef";

    std::cout << s.substr(2, 3) << '\n'; // cde
    std::cout << s.substr(2) << '\n';    // cdef，默认取到末尾
    std::cout << s.substr(4, 100) << '\n'; // ef，长度会被末尾截断
}
```

## 与“结束下标”区分

如果你想取左闭右开区间 `[left, right)`，长度应写成：

```cpp
std::string part = s.substr(left, right - left);
```

例如取下标 `[1, 4)`：

```cpp
std::string part = s.substr(1, 4 - 1); // bcd
```

## 常见错误与边界

### 把第二个参数写成结束下标

```cpp
std::string s = "abcdef";
std::string part = s.substr(2, 4); // 实际含义：从2开始取4个字符，结果是 cdef
// 如果想取左闭右开区间 [2, 4)，应写成 s.substr(2, 4 - 2)，结果是 cd
```

用更容易暴露“长度不是结束下标”的例子：

```cpp
std::string s = "abcdef";
std::string part = s.substr(2, 5); // 从2开始取5个，但实际只剩4个，结果仍是 cdef
```

### 起始下标超出范围

```cpp
std::string s = "abc";
// s.substr(4, 1); // 抛出 std::out_of_range
```

`pos == s.size()`时可以得到空字符串；`pos > s.size()`会超出有效起点范围。

## 如何验证

对 `std::string s = "abcdef"` 输出 `s.substr(2, 3)`，应为 `cde`，第二个参数是长度而不是结束下标。再尝试 `s.substr(6)`，应得到空串；起始位置大于 6 时会抛出范围异常，调用前要确认下标。

## 使用规则

1. 先写清起点，再计算长度。
2. 左闭右开区间 `[l, r)`使用 `substr(l, r - l)`。
3. 长度超过剩余字符数时会截到末尾。
4. `pos == size()`返回空字符串。
5. `pos > size()`属于错误边界，需要提前保证起点合法。

## 关联

string 初始化 · 字符串去空格 · 数组下标边界

## 来源

- C++ 工作草案，[basic_string::substr]：`substr(size_type pos, size_type n)`接口和行为。<https://eel.is/c++draft/string.substr>
- 本地参考资料：Haotian-BiJi STL 原始资料
