---
title: Node 指针与 int 指针：指向的类型决定什么
tags:
  - c-plus-plus
  - pointer
  - struct
  - linked-list
created: 2026-08-10
---

# Node 指针与 int 指针：指向的类型决定什么

## 一句话结论

`Node* p` 和 `int* p` 都是指针变量。区别不在星号，而在 `*` 左边的类型：前者指向一个完整的 `Node` 结构体对象，后者只指向一个 `int` 整数。指向的类型决定了解引用后得到什么、能访问什么成员、指针运算的步长以及能传给哪些函数。

## 最小示例

```cpp
struct Node {
    int data;
    Node* next;
};

int main() {
    int number = 10;
    int* ip = &number;

    Node first{1, nullptr};
    Node* np = &first;
}
```

变量的实际类型如下：

| 表达式 | 类型 | 含义 |
| --- | --- | --- |
| `ip` | `int*` | 指向一个 `int` |
| `*ip` | `int` | 取出该整数 |
| `np` | `Node*` | 指向一个 `Node` |
| `*np` | `Node` | 取出该结构体对象 |
| `np->data` | `int` | 访问该节点的数据成员 |
| `np->next` | `Node*` | 访问下一个节点的指针 |

## 更准确的理解

你的理解基本正确，但“`int*` 一般一一对应”需要修正。

- `Node*` 指向一个 `Node` 对象。这个对象由多个成员组成，例如 `data` 和 `next`；通过一个地址可以访问该节点的整体及其成员。
- `int*` 指向一个 `int` 对象。单个 `int` 只有一个整数值，不能访问结构体成员。
- `int*` 也可以指向 `int` 数组的第一个元素，因此它不必只对应一个整数；它可以借助下标或指针运算访问数组中的多个连续 `int`。

```cpp
int values[3]{10, 20, 30};
int* ip = values;

ip[0] = 11;
ip[1] = 21;
```

因此，关键不在“一个地址能否对应多个变量”，而在“该地址指向的**一个对象**是什么类型”。`Node` 对象本身包含多个成员，`int` 对象本身只表示一个整数；数组则由多个同类型对象连续组成。
## 为什么链表需要 `Node*`

链表的一个节点不仅有一个数据，还需要知道“下一个节点在哪里”。因此节点内部保存的应是指向节点的指针：

```cpp
struct Node {
    int data;
    Node* next;
};
```

`next` 必须写成 `Node*`，因为下一个元素也是一个 `Node`。

如果写成：

```cpp
int* next;
```

它只能指向一个整数，无法表示“下一个完整节点”，因此也不能通过 `next->data` 或 `next->next` 继续访问链表。

## 成员访问的差异

```cpp
Node first{1, nullptr};
Node* np = &first;

int value = 10;
int* ip = &value;

np->data = 2;  // 正确：np 指向 Node，Node 有 data 成员
*ip = 20;      // 正确：ip 指向 int，解引用后就是一个整数

// ip->data = 2; // 错误：int 没有 data 成员
```

`p->member` 是 `(*p).member` 的简写。只有当 `*p` 是具有该成员的结构体或类对象时，才能使用 `->`。

## 类型还影响指针运算

假设指针指向数组元素：

```cpp
int values[3]{10, 20, 30};
int* ip = values;

Node nodes[3]{};
Node* np = nodes;

++ip; // 移到下一个 int
++np; // 移到下一个 Node
```

两次 `++` 都移动到“下一个元素”，但移动的字节数由目标类型决定。`int*` 按一个 `int` 的大小前进，`Node*` 按一个 `Node` 的大小前进。不要把这个规则理解成固定的 4 字节或 8 字节，具体对象大小由实现和成员布局决定。

## 不能随意互换

```cpp
void printNumber(int* p);
void printNode(Node* p);
```

`int*` 只能传给期望 `int*` 的接口，`Node*` 只能传给期望 `Node*` 的接口。强行转换指针类型通常只是欺骗编译器；按错误类型解引用会导致未定义行为。

## 一个容易混淆的点

```cpp
Node* p;
```

这只声明了一个“可存放 Node 地址的变量”，没有创建 `Node` 对象，也没有让 `p` 指向有效对象。应当初始化：

```cpp
Node* p = nullptr; // 当前不指向任何节点
```

或让它指向一个已存在的节点：

```cpp
Node first{1, nullptr};
Node* p = &first;
```

## 使用规则

1. 读 `T* p` 时，先读成“`p` 是指向 `T` 的指针”。
2. `*p` 得到 `T` 类型的对象；`p->member` 访问该对象的成员。
3. 链表中“下一个节点”的类型应为 `Node*`，不是 `int*`。
4. 指针变量声明后先初始化为有效对象地址或 `nullptr`。
5. 不要用强制类型转换把 `int*` 伪装成 `Node*`，或反过来。

## 关联

C++ 学习笔记 · NULL 与 nullptr：空指针的现代写法 · delete：销毁动态对象并归还内存 · C++ 笔记写作方法论与要求

## 来源

- C++ 工作草案，[dcl.ptr]：`T*` 声明为“指向 T 的指针”。<https://eel.is/c++draft/dcl.ptr>
- C++ 工作草案，[basic.types]：对象指针类型和“pointer to T”的定义。<https://eel.is/c++draft/basic.types>
