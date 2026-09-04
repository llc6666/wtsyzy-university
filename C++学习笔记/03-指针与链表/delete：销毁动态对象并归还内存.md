---
title: delete：销毁动态对象并归还内存
tags:
  - c-plus-plus
  - pointer
  - memory-management
created: 2026-08-10
---

# delete：销毁动态对象并归还内存

## 一句话结论

`delete` 用于结束由 `new` 创建的单个动态对象的生命周期，并释放对应的动态存储。数组必须使用 `delete[]`。它不应用于栈对象、全局对象或不由 `new` 得到的地址。

## 它具体做了什么

对于一个有效的、由 `new` 创建的对象，`delete p;` 的逻辑分为两步：

1. 调用对象的析构函数，结束对象生命周期。
2. 调用对应的释放函数，归还动态存储。

```cpp
class FileGuard {
public:
    ~FileGuard() {
        // 在这里释放对象持有的文件、锁等资源
    }
};

int main() {
    FileGuard* guard = new FileGuard;
    delete guard; // 先执行 ~FileGuard()，再释放这块动态存储
}
```

## 单对象与数组必须配对

```cpp
int* one = new int{7};
delete one;

int* many = new int[3]{1, 2, 3};
delete[] many;
```

配对规则：

| 分配方式 | 正确释放方式 |
| --- | --- |
| `new T` | `delete p` |
| `new T[n]` | `delete[] p` |

把 `new[]` 得到的指针交给 `delete`，或把 `new` 得到的指针交给 `delete[]`，行为未定义。

## `delete nullptr` 可以吗

可以。删除空指针没有效果：

```cpp
int* p = nullptr;
delete p; // 安全，无操作
```

因此不需要为 `delete` 专门写 `if (p != nullptr)`。

## 三类高风险错误

### 1. 删除非动态对象

```cpp
int value = 10;
int* p = &value;
// delete p; // 错误：value 不由 new 创建
```

### 2. 重复删除

```cpp
int* p = new int{10};
delete p;
// delete p; // 错误：同一块存储再次被释放
```

第一次 `delete` 后，指针变量通常仍保存旧地址。可以将当前变量置空，降低误用风险：

```cpp
delete p;
p = nullptr;
```

这无法让其他指向同一对象的指针自动安全。

### 3. 使用已删除对象

```cpp
int* p = new int{10};
delete p;
// std::cout << *p; // 错误：对象生命周期已结束
```

## 更好的默认方案：避免手写 `new` 和 `delete`

现代 C++ 的默认选择是自动管理资源：

```cpp
#include <memory>

int main() {
    auto p = std::make_unique<int>(42);
    // 离开作用域时，unique_ptr 自动删除对象
}
```

`std::unique_ptr` 明确独占所有权，能在异常和多分支控制流中自动释放对象。需要共享所有权时再谨慎使用 `std::shared_ptr`，不要因为方便而默认共享。

## 如何验证

对 `new` 得到的单对象执行一次 `delete`，对 `new[]` 得到的数组执行一次 `delete[]`，并让析构函数输出编号以确认每个数组元素都被销毁。不要通过“删除后再读取指针”验证，读取已释放对象本身就是未定义行为。

## 使用边界

- 只有“谁创建，谁负责释放”的所有权关系明确时，手写 `delete` 才有讨论空间。
- `delete` 负责动态存储，不负责所有资源设计。对象析构函数是否正确关闭文件、释放锁或断开连接，取决于类本身的实现。
- `malloc`/`free` 与 `new`/`delete` 属于两套机制，不能交叉配对。

## 关联

NULL 与 nullptr：空指针的现代写法 · C++ 学习笔记 · C++ 笔记写作方法论与要求

## 来源

- C++ 工作草案，[expr.delete]：`delete`、`delete[]`、析构与释放函数的语义。<https://eel.is/c++draft/expr.delete>
- C++ 工作草案，[basic.stc.dynamic.deallocation]：动态存储释放规则。<https://eel.is/c++draft/basic.stc.dynamic.deallocation>
- C++ 工作草案，[util.smartptr.unique]：`unique_ptr` 的所有权与析构语义。<https://eel.is/c++draft/util.smartptr.unique>
