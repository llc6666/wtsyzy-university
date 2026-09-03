---
title: string 拼接：operator+ 与 += 的性能边界
tags:
  - c-plus-plus
  - stl
  - string
  - performance
created: 2026-08-27
---

# string 拼接：operator+ 与 += 的性能边界

## 一句话结论

`+`创建拼接结果，`+=`把内容追加到已有字符串。循环中反复写 `s = s + part`可能反复创建和复制临时字符串；持续追加时优先写 `s += part`，已知规模时可配合 `reserve`。

## 它解决什么问题

下面两段代码最终结果相同，但执行过程可能不同：

```cpp
s = s + "a";
s += "a";
```

区别在于：第一句先计算一个新的拼接结果，再把结果赋回 `s`；第二句直接调用追加操作。

## 工作机制

```cpp
std::string a = "ab";
std::string b = "cd";
std::string c = a + b; // 创建内容为 abcd 的结果
a += b;                // 把 b 追加到 a，a 变为 abcd
```

当代码位于循环中时，`s = s + part`每轮都可能：

1. 分配或准备新的存储空间。
2. 复制旧字符串内容。
3. 复制追加内容。
4. 把结果赋回原对象。

如果字符串逐轮变长，旧内容可能被反复复制，整体成本可能达到二次级别。具体表现会受实现、容量增长和短字符串优化影响，不能用一次机器测试结果替代复杂度分析。

## 最小示例

```cpp
#include <iostream>
#include <string>

int main() {
    std::string byPlus;
    std::string byAppend;

    for (int i = 0; i < 5; ++i) {
        byPlus = byPlus + "a";
        byAppend += "a";
    }

    std::cout << byPlus << '\n';   // aaaaa
    std::cout << byAppend << '\n'; // aaaaa
}
```

## 推荐写法

```cpp
std::string result;
result.reserve(1000); // 已知大致总长度时减少重新分配的可能

for (int i = 0; i < 1000; ++i) {
    result += 'x';
}
```

`reserve`只预留容量，不改变 `size()`；它是性能提示，不能替代向字符串中追加元素。

## 常见错误与边界

### 把 `reserve`当成扩容后的可写长度

```cpp
std::string s;
s.reserve(10);
// s[0] = 'a'; // 错误：s.size()仍然是 0
s.push_back('a');
```

### 把“通常更快”说成“永远更快”

`+=`适合连续追加，但实际速度还受字符串长度、容量策略和实现影响。正确结论是：连续追加时，`+=`表达的操作更直接，通常能避免 `+`带来的额外临时结果。

### `+`并非不能使用

少量、固定次数的拼接很适合用 `+`表达：

```cpp
std::string message = first + ": " + second;
```

需要重点警惕的是在大循环中不断把增长中的字符串与新内容相加。

## 使用规则

1. 固定数量的少量拼接可以使用 `+`。
2. 循环追加优先使用 `+=`或`append`。
3. 已知大致总长度时考虑 `reserve`。
4. `reserve`改变容量，不改变元素数量。
5. 不依据单次计时武断判断复杂度，应观察是否反复复制增长中的旧内容。

## 关联

string 初始化 · string 的 substr · C++ 笔记写作方法论

## 来源

- C++ 工作草案，[basic_string]：`operator+=`、`append`和字符串修改接口。<https://eel.is/c++draft/basic.string>
- 本地参考资料：Haotian-BiJi STL 原始资料
