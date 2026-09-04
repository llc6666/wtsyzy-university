---
title: Queue dequeue：出队逻辑与尾指针边界
tags:
  - c-plus-plus
  - queue
  - linked-list
  - pointer
created: 2026-08-14
---

# Queue dequeue：出队逻辑与尾指针边界

## 一句话结论

`dequeue()` 从队列头部取出一个节点，保存数据，移动 `head`，删除旧节点，并返回数据。删除最后一个节点后，必须同时令 `tail = nullptr`，否则 `tail` 会指向已经释放的内存。

## 原代码的逻辑

```cpp
int Queue::dequeue(){
    Node *p=head;
    if(head==NULL){
        tail=NULL;
        return -1;
    }
    int d=head->data;
    head=head->next;
    delete p;
    return d;
}
```

1. `Node* p = head`：保存当前头节点地址，后续用于删除它。
2. `if (head == NULL)`：如果队列为空，让 `tail` 也为空，并返回 `-1`。
3. `int d = head->data`：先保存头节点数据，不能等删除后再读取。
4. `head = head->next`：头指针向后移动，第二个节点成为新的头节点。
5. `delete p`：释放原来的头节点。删除后不能再访问 `p->data` 或 `p->next`。
6. `return d`：返回被取出节点的数据。

现代 C++ 建议将 `NULL` 改为 `nullptr`。

## 原代码的关键漏洞

当队列只有一个节点时：

```text
head -> node <- tail
```

执行 `head = head->next` 后，`head == nullptr`。原代码没有更新 `tail`，于是 `tail` 仍然指向已经被 `delete` 的节点，形成悬空指针。

## 推荐写法

```cpp
int Queue::dequeue() {
    if (head == nullptr) {
        tail = nullptr;
        return -1;
    }

    Node* p = head;
    int d = p->data;

    head = p->next;

    if (head == nullptr) {
        // 删除的是最后一个节点，队列现在为空
        tail = nullptr;
    }

    delete p;
    return d;
}
```

## 三种情况

### 空队列

```text
head == nullptr
```

不删除节点，返回 `-1`。

### 只有一个节点

```text
head -> node <- tail
```

删除后必须是：

```text
head == nullptr
tail == nullptr
```

### 多个节点

```text
head -> A -> B -> C <- tail
```

删除 A 后：

```text
head -> B -> C <- tail
```

此时 `tail` 不变。

## 返回 `-1` 的边界

如果队列允许存储 `-1`，用 `-1` 表示“队列为空”会产生歧义。更稳妥的接口是返回成功状态，并用引用参数返回数据：

```cpp
bool Queue::dequeue(int& result) {
    if (head == nullptr) {
        tail = nullptr;
        return false;
    }

    Node* p = head;
    result = p->data;
    head = p->next;

    if (head == nullptr) {
        tail = nullptr;
    }

    delete p;
    return true;
}
```

## 如何验证

按顺序入队 `10, 20` 后连续出队三次。前两次应返回 `10`、`20`，第三次走空队列分支；第二次出队结束后同时检查 `head` 和 `tail` 都是 `nullptr`。只检查返回值而不检查尾指针，无法发现悬空尾指针问题。

## 使用规则

1. 先判断队列是否为空，再访问 `head->data`。
2. 先保存数据和下一节点地址，再删除当前节点。
3. 删除后不要再访问被删除节点的成员。
4. 删除最后一个节点时，同时令 `head` 和 `tail` 为 `nullptr`。
5. 空队列应保持 `head == nullptr && tail == nullptr`。
6. 若数据允许 `-1`，不要用 `-1` 同时表达“取值失败”。

## 关联

C++ 学习笔记 · 析构函数：对象销毁时的自动清理 · delete：销毁动态对象并归还内存 · List 的 size 与 find：链表遍历指针的使用

## 来源

- C++ 工作草案，[expr.delete]：`delete` 表达式结束对象生命周期并释放动态存储。<https://eel.is/c++draft/expr.delete>
- C++ 工作草案，[basic.life]：对象生命周期规则。<https://eel.is/c++draft/basic.life>
