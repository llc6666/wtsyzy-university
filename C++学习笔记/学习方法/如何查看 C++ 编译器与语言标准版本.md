---
title: 如何查看 C++ 编译器与语言标准版本
tags:
  - c-plus-plus
  - compiler
  - version
  - dev-cpp
created: 2026-08-26
---

# 如何查看 C++ 编译器与语言标准版本

## 先区分两个版本

“C++ 版本”可能指两个不同东西：

1. **编译器版本**：GCC、Clang 或 MSVC 本身的版本。
2. **语言标准版本**：当前代码按 C++98、C++11、C++14、C++17、C++20 等哪个标准编译。

编译器较新，不代表当前项目一定启用了较新的 C++ 标准。真正决定 `nullptr`、范围 `for` 等语法能否使用的，是实际编译命令中的语言标准选项。

## 查看 GCC 版本

在终端执行：

```text
g++ --version
```

也可以执行：

```text
g++ -v
```

GCC 使用 `-std=` 选项选择 C++ 语言方言，例如：

```text
g++ -std=c++11 main.cpp -o main
```

也可以使用：

```text
-std=c++17
-std=c++20
```

GCC 官方文档明确区分了编译器版本和通过 `-std=` 选择的语言标准。<https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html>

## 查看 Clang 版本

```text
clang++ --version
```

选择标准的写法通常也是：

```text
clang++ -std=c++17 main.cpp -o main
```

## 在 Dev-C++ 中查看

Dev-C++ 只是集成开发环境，实际编译通常由它配置的 MinGW/GCC 完成。

可以按以下方式检查：

1. 打开“工具”或“编译器选项”。
2. 找到编译器设置、代码生成或语言标准相关选项。
3. 查看是否有 `-std=c++11`、`-std=c++14`、`-std=c++17` 等参数。
4. 编译一次后查看编译日志，确认实际执行的 `g++` 命令中有对应的 `-std=` 参数。
5. 若没有语言标准选项，可以在附加编译参数中加入：

```text
-std=c++11
```

旧版 Dev-C++ 的菜单名称可能不同，因此编译日志比界面名称更可靠。

## 用程序查看当前启用的标准

新建一个 `.cpp` 文件：

```cpp
#include <iostream>

int main() {
    std::cout << "__cplusplus = " << __cplusplus << '\n';
}
```

常见结果：

| 输出值 | 对应标准 |
| ---: | --- |
| `199711L` | C++98/C++03 时代 |
| `201103L` | C++11 |
| `201402L` | C++14 |
| `201703L` | C++17 |
| `202002L` | C++20 |
| `202302L` | C++23 |

这个宏反映的是当前编译命令选择的语言模式，不是“编译器理论上最高支持的版本”。

## 用特性测试验证

只看宏还不够时，可以直接测试语法：

```cpp
#include <iostream>
#include <string>

int main() {
    std::string text = "abc";

    for (char ch : text) {
        std::cout << ch;
    }
}
```

这段代码需要 C++11 的范围 `for`。如果 `nullptr`、范围 `for` 或 `std::make_unique` 报错，先检查实际 `-std=` 设置和编译器版本。

## 结论

最可靠的检查顺序：

```text
1. g++ --version       查看编译器版本
2. 查看编译日志         确认实际使用的编译器和参数
3. 运行 __cplusplus     确认当前语言标准
4. 明确设置 -std=c++17  避免依赖编译器默认值
```

## 关联

C++ 学习笔记 · NULL 与 nullptr：空指针的现代写法 · 字符串输入：getline 读取整行 · C++ 笔记写作方法论与要求
