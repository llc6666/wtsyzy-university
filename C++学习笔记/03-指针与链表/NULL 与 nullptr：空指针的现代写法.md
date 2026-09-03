---
title: NULL 与 nullptr：空指针的现代写法
tags:
  - c-plus-plus
  - pointer
  - language-rule
created: 2026-08-10
---

# NULL 与 nullptr：空指针的现代写法

## 一句话结论

在现代 C++ 中，表示“这里没有指向任何对象”的指针值，应使用 `nullptr`。`NULL` 是为兼容旧代码保留的宏，具体定义由实现决定，容易和整数 `0` 混淆。

## 先区分三个概念

| 名称 | 含义 |
| --- | --- |
| 空指针值 | 不指向任何对象或函数的指针值 |
| `nullptr` | C++11 引入的空指针字面量，类型为 `std::nullptr_t` |
| `NULL` | 宏，展开结果由实现定义，是旧式空指针常量写法 |

`nullptr` 的价值在于类型明确：它表达“空指针”，不表达普通整数零。

## 最小示例

```cpp
int* p = nullptr;  // p 当前不指向 int 对象

if (p == nullptr) {
    // 先判断，再决定是否使用 p
}
```

## `NULL` 为什么容易出问题

`NULL` 在许多环境中会表现得像整数 `0`。当函数重载同时接受整数和指针时，代码意图可能变得不清楚，甚至选择了错误的重载。

```cpp
#include <iostream>

void show(int)  { std::cout << "int\n"; }
void show(int*) { std::cout << "pointer\n"; }

int main() {
    show(nullptr); // 明确调用 show(int*)
    // show(NULL); // 依赖 NULL 的实现定义，可能含糊或偏向整数重载
}
```

上例的重点不在于所有编译器都会报错，而在于 `NULL` 的含义依赖实现，`nullptr` 的意图稳定且可由类型系统识别。

## `nullptr` 不等于“无效指针”

`nullptr` 表示没有目标。

悬空指针则曾经指向一个对象，但该对象的生命周期已经结束：

```cpp
int* p = new int{42};
delete p;
p = nullptr; // 当前这个变量不再保存悬空地址
```

若删除后不赋值，`p` 仍保存旧地址。它不等于 `nullptr`，继续解引用会产生未定义行为。

## 在 Dev-C++ 中报错的排查

`nullptr` 是 C++11 才引入的关键字。Dev-C++ 只是 IDE；真正决定能否识别它的是当前项目使用的编译器、语言标准选项和源文件类型。

按以下顺序检查：

1. **确认文件扩展名是 `.cpp`。** 若文件是 `.c`，Dev-C++ 会按 C 语言编译。旧版 C 编译器不认识 `nullptr`。
2. **确认项目以 C++11 或更高标准编译。** 在 Dev-C++ 的编译器或项目选项中，查找 `Language standard`、`-std` 或“附加命令行参数”，设置为 `-std=c++11`、`-std=gnu++11` 或更高版本，例如 `-std=c++17`。
3. **查看编译日志。** 日志中应能看到实际执行的 `g++` 命令及 `-std=...` 参数。只修改全局设置而项目配置覆盖它时，最终命令可能仍是旧标准。
4. **若已启用 C++11 仍报错，检查编译器版本。** 某些旧版 Dev-C++ 捆绑的 GCC 很旧，无法完整支持 C++11；此时应更换较新的编译器或 IDE。

最小测试：

```cpp
int main() {
    int* p = nullptr;
    return p == nullptr ? 0 : 1;
}
```

若错误信息接近 `nullptr was not declared in this scope` 或 `nullptr 未声明`，通常是第 1、2 或 4 项的问题，而不是缺少头文件。`nullptr` 是语言关键字，不需要 `#include`。

若课程或项目被明确限制为 C++03，只能使用旧式空指针常量，例如：

```cpp
int* p = 0; // 仅用于兼容 C++03 的旧代码
```

这是一种兼容措施，不应带回现代 C++ 项目。
## 使用规则

1. C++11 及以后：用 `nullptr` 初始化和比较指针。
2. 不用 `0` 或 `NULL` 表示空指针，除非维护必须兼容的旧接口。
3. 调用成员函数或解引用前，只有在“指针可能为空”的语义下才检查 `nullptr`；不要用无意义的检查掩盖所有权设计问题。
4. `nullptr` 不能修复悬空指针、越界指针或野指针，它只表示空。

## 关联

delete：销毁动态对象并归还内存 · C++ 学习笔记 · C++ 笔记写作方法论与要求

## 来源

- C++ 工作草案，[support.types.nullptr]：`std::nullptr_t` 与空指针字面量。<https://eel.is/c++draft/support.types.nullptr>
- C++ 工作草案，[conv.ptr]：空指针常量转换为指针值。<https://eel.is/c++draft/conv.ptr>
- C++ 工作草案，[support.types.layout]：`NULL` 的实现定义要求。<https://eel.is/c++draft/support.types.layout>
