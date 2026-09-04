---
title: 结构体自引用指针与 bool 数据成员
tags:
  - c-plus-plus
  - struct
  - pointer
  - bool
created: 2026-08-16
---

# 结构体自引用指针与 bool 数据成员

## 原代码的问题

```cpp
struct que {
    que next, pre;
    bool data;
    que(int d) : next(), pre(), data(d) {}
};
```

`data(d)` 确实会把 `int d` 转换为 `bool`：`0` 变为 `false`，任何非零值变为 `true`。但 `que next, pre;` 本身无法成立，因为它要求每个 `que` 对象里面再直接放两个完整的 `que` 对象，进而无限嵌套，类型大小无法确定。

## 机制

结构体定义尚未结束时，编译器不知道它的完整大小，因此成员不能直接嵌入同类型对象；指针本身大小固定，`Node* next` 只保存另一个节点的地址，故可以自引用。`bool` 成员保存真假状态，不会自动成为函数。

## 正确的双向链表节点写法

```cpp
struct Que {
    Que* next;
    Que* pre;
    bool data;

    Que(int d)
        : next(nullptr),
          pre(nullptr),
          data(d != 0) {}
};
```

`next` 和 `pre` 保存其他节点的地址，而不是把其他节点完整嵌入当前节点。指针类型可以指向尚未完成定义的同类型对象；直接按值包含同类型对象则需要完整类型和确定大小。

## `data(d)` 的具体结果

```cpp
Que a(0);   // a.data == false
Que b(1);   // b.data == true
Que c(2);   // c.data == true
Que d(-5);  // d.data == true
```

如果这个字段本来要保存整数，应该写成：

```cpp
struct Que {
    Que* next;
    Que* pre;
    int data;

    Que(int d)
        : next(nullptr), pre(nullptr), data(d) {}
};
```

如果它确实只表达“有/无”“开/关”“真假”，建议让接口直接接收 `bool`：

```cpp
struct Que {
    Que* next{};
    Que* pre{};
    bool data{};

    explicit Que(bool value)
        : data(value) {}
};
```

如果调用 `Que q(2);`，这个版本仍可能发生 `int` 到 `bool` 的转换。若希望明确把整数规范化为真假，可以写：

```cpp
Que(int d)
    : next(nullptr), pre(nullptr), data(d != 0) {}
```

## `que *next, *pre;` 可以吗

可以。它等价于：

```cpp
que* next;
que* pre;
```

同一条声明中的多个声明符都继承前面的 `que*` 类型。为了避免初学时误读，也可以拆成两行。C++ 标准的声明规则允许一个声明中列出多个声明符，`*` 修饰各自的变量名。

```cpp
struct que {
    que* next;
    que* pre;
    bool data;

    que(int d)
        : next(nullptr), pre(nullptr), data(d != 0) {}
};
```

注意 `*` 绑定到变量名，而不是简单地绑定到类型名。因此下面的写法容易误读：

```cpp
que* next, pre; // next 是 que*，pre 是 que，不是两个指针
```

必须写成：

```cpp
que *next, *pre;
```

或者更推荐拆开：

```cpp
que* next;
que* pre;
```
## `next()`、`pre()` 与 `nullptr`

在原写法中：

```cpp
next(), pre()
```

意图是值初始化成员。改为指针后，推荐直接写：

```cpp
next(nullptr), pre(nullptr)
```

这样明确表示新节点暂时没有前驱和后继。

## 如何验证

定义两个节点 `a`、`b`，令 `a.next = &b`、`b.pre = &a`，再输出 `a.next->data` 与 `b.pre->data`，应分别读到对方节点的数据。将 `next` 初始化为 `nullptr` 后，访问前必须先判断非空；不要写成函数调用形式 `next()`。

## 使用规则

1. 双向链表节点应保存 `Node* next` 和 `Node* pre`，不要按值保存同类型节点。
2. `int` 赋给 `bool` 时，零为假，非零为真，数值本身不会被保留。
3. 要保存整数就使用 `int data`；只表示真假才使用 `bool data`。
4. 初始化链表指针时优先写 `nullptr`。
5. `data(d)` 是隐式转换；需要明确语义时写 `data(d != 0)`。

## 关联

C++ 学习笔记 · Node 指针与 int 指针：指向的类型决定什么 · NULL 与 nullptr：空指针的现代写法

## 来源

- C++ 工作草案，[conv.bool]：整数到 `bool` 的转换规则。<https://eel.is/c++draft/conv.bool>
- C++ 工作草案，[basic.types]：不完整类型与对象大小规则。<https://eel.is/c++draft/basic.types>
- C++ 工作草案，[class]：类对象由成员子对象组成。<https://eel.is/c++draft/class>
