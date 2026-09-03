---
title: List 的 size 与 find：链表遍历指针的使用
tags:
  - c-plus-plus
  - linked-list
  - pointer
  - member-function
created: 2026-08-11
---

# List 的 size 与 find：链表遍历指针的使用

## 一句话结论

在 `int List::size()` 与 `int List::find()` 中，`Node* p = head;` 应作为每次调用时新建的**局部遍历指针**。移动 `p` 只是在移动这次遍历的副本，不会改变链表头指针 `head`；前提是不要写 `head = ...`，也不要删除节点。

## 一个典型链表

```cpp
struct Node {
    int data;
    Node* next;
};

class List {
private:
    Node* head = nullptr;

public:
    int size() const;
    int find(int target) const;
};
```

## `size()`：每访问一个节点就计数

```cpp
int List::size() const {
    int count = 0;
    const Node* p = head;

    while (p != nullptr) {
        ++count;
        p = p->next;
    }
    return count;
}
```

这里的 `p` 从 `head` 开始，依次走向下一个节点。`p = p->next` 只改变局部变量 `p`，不会改变成员变量 `head`。

## `find()`：每访问一个节点就比较

以下约定：找到时返回第一个匹配节点的下标，找不到返回 `-1`。

```cpp
int List::find(int target) const {
    int index = 0;
    const Node* p = head;

    while (p != nullptr) {
        if (p->data == target) {
            return index;
        }
        p = p->next;
        ++index;
    }
    return -1;
}
```

`find` 的返回约定必须先定清楚：若存在重复值，上面的代码返回第一个匹配项。若要找最后一个或所有匹配项，需要另一种实现。

## 必须注意的五件事

### 1. 只有需要遍历链表的函数，才各自初始化遍历指针

`size()`、`find()`、`print()`、`clear()` 等需要从节点向后走的函数，通常都在自己的函数体中声明：

```cpp
Node* p = head;
```

这不是“每个构造函数都必须定义”。`size()` 和 `find()` 是**成员函数**，不是构造函数。构造函数名称与类名相同且没有返回类型，例如：

```cpp
List::List() : head(nullptr) {}
```

构造函数的首要任务是初始化链表对象。只有它确实需要创建、复制或连接节点时，才可能另外使用局部 `Node*`。

不需要遍历的成员函数不必声明 `p`：

```cpp
bool List::empty() const {
    return head == nullptr;
}
```


```cpp
Node* p = head;
```

`size()` 中的 `p` 和 `find()` 中的 `p` 是两个不同的局部变量。一个函数结束后，另一个函数不会接着它的位置继续遍历。

### 2. 先判断空指针，再访问成员

正确：

```cpp
while (p != nullptr) {
    int value = p->data;
    p = p->next;
}
```

错误：

```cpp
// int value = p->data; // p 可能是 nullptr
```

空链表中 `head == nullptr`。上面的 `while` 会直接跳过，`size()` 返回 `0`，`find()` 返回 `-1`。

### 3. 遍历函数不要改动 `head`

下面的写法会破坏链表入口：

```cpp
// 错误示例
while (head != nullptr) {
    head = head->next;
}
```

循环结束后，`head` 变为 `nullptr`，原链表无法再通过头指针访问。遍历应移动 `p`，而不是移动 `head`。

### 4. `size` 和 `find` 不负责释放节点

这两个函数的职责是读取和查找。不要在其中写：

```cpp
// delete p; // 错误：会破坏链表，并可能让后续访问成为悬空指针
```

释放节点应由析构函数、`clear()` 或明确的删除操作完成。

### 5. 只读函数优先写为 `const`

```cpp
int size() const;
int find(int target) const;
```

并使用：

```cpp
const Node* p = head;
```

这样编译器会阻止你在 `size()`、`find()` 中意外修改节点内容。若课程代码尚未使用 `const`，可以先写 `Node* p = head;`，但应保持只读，不修改 `data`、`next` 或 `head`。

## 两个额外边界

- 以上 `while (p != nullptr)` 只适用于普通单链表。循环链表没有 `nullptr` 结尾，需要以“是否回到 `head`”作为停止条件。
- 这两个实现每次都要扫描链表，时间复杂度为 O(n)。频繁调用 `size()` 时，可以维护节点数量成员，但插入和删除时必须同步更新它。

## 关联

C++ 学习笔记 · Node 指针与 int 指针：指向的类型决定什么 · 析构函数：对象销毁时的自动清理 · 函数返回值：return 交回什么

## 来源

- C++ 工作草案，[dcl.ptr]：对象指针声明与类型。<https://eel.is/c++draft/dcl.ptr>
- C++ 工作草案，[class.this]：成员函数访问所属对象的成员。<https://eel.is/c++draft/class.this>
- C++ 工作草案，[expr.ref]：通过 `->` 访问指针所指对象的成员。<https://eel.is/c++draft/expr.ref>
