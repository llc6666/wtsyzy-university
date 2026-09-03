---
title: 二维数组按列排序：vector、array 与间接排序
tags:
  - c-plus-plus
  - stl
  - sorting
  - array
created: 2026-09-01
---

# 二维数组按列排序：vector、array 与间接排序

## 一句话结论

按二维数据的某一列排序，本质是“比较每一行的第 `k` 个元素”；优先使用 `vector<array<T, M>>`、结构体或索引数组，避免把二维内存布局和排序对象混在一起。

## 头文件

```cpp
#include <algorithm>  // std::sort
#include <array>      // std::array
#include <iostream>   // std::cout
#include <vector>     // std::vector
```

## 1. `vector<vector<int>>`

适合行数和每行长度都需要灵活变化的情况。以下示例假设每一行都至少有 `k + 1` 个元素：

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<std::vector<int> > arr;
    arr.push_back(std::vector<int>{3, 5, 1});
    arr.push_back(std::vector<int>{1, 2, 9});
    arr.push_back(std::vector<int>{4, 1, 7});

    int k = 1;

    std::sort(arr.begin(), arr.end(),
        [k](const std::vector<int>& a,
            const std::vector<int>& b) {
            return a[k] < b[k];
        });

    for (std::size_t i = 0; i < arr.size(); ++i) {
        for (std::size_t j = 0; j < arr[i].size(); ++j) {
            std::cout << arr[i][j] << ' ';
        }
        std::cout << '\n';
    }
}
```

按第 `1` 列升序后的结果：

```text
4 1 7
1 2 9
3 5 1
```

降序只需改成：

```cpp
return a[k] > b[k];
```

### 边界

如果各行长度不一致，直接访问 `a[k]` 可能越界。比较前必须保证：

```cpp
k < a.size() && k < b.size()
```

或者从输入设计上保证这是一个规则矩阵。

## 2. `vector<array<int, M>>`

当每行列数固定时，`std::array` 比 `vector<vector<int>>` 更适合表达固定长度的行：

```cpp
#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

int main() {
    std::vector<std::array<int, 3> > arr;
    arr.push_back(std::array<int, 3>{{3, 5, 1}});
    arr.push_back(std::array<int, 3>{{1, 2, 9}});
    arr.push_back(std::array<int, 3>{{4, 1, 7}});

    int k = 2;

    std::sort(arr.begin(), arr.end(),
        [k](const std::array<int, 3>& a,
            const std::array<int, 3>& b) {
            return a[k] < b[k];
        });

    for (std::size_t i = 0; i < arr.size(); ++i) {
        for (std::size_t j = 0; j < arr[i].size(); ++j) {
            std::cout << arr[i][j] << ' ';
        }
        std::cout << '\n';
    }
}
```

`std::array<int, 3>` 的长度固定为 3，行本身可以作为 `std::sort` 的元素进行交换。

## 3. 原生二维数组：索引数组间接排序

如果原始数据必须保持在：

```cpp
int arr[N][M];
```

可以只排序行号，不移动原始数据：

```cpp
#include <algorithm>
#include <iostream>

int main() {
    const int N = 3;
    const int M = 3;

    int arr[N][M] = {
        {3, 5, 1},
        {1, 2, 9},
        {4, 1, 7}
    };

    int k = 1;
    int index[N] = {0, 1, 2};

    std::sort(index, index + N,
        [&arr, k](int i, int j) {
            return arr[i][k] < arr[j][k];
        });

    for (int p = 0; p < N; ++p) {
        int row = index[p];
        for (int col = 0; col < M; ++col) {
            std::cout << arr[row][col] << ' ';
        }
        std::cout << '\n';
    }
}
```

`index` 排序后可能变成：

```text
2 0 1
```

它表示输出顺序为原来的第 2 行、第 0 行、第 1 行。原始 `arr` 的内容没有改变。

这种方式称为间接排序，适合：

- 不希望移动原始数据。
- 需要保留原始行号。
- 原始数组来自题目或固定接口，不能改成 `vector`。

如果要生成一个真正排好序的新数组，再按照 `index` 复制即可。

## 4. 原生二维数组直接按行排序

原生二维数组的每一行是固定长度数组。部分现代 C++ 编译器可以通过数组引用比较器直接对行排序：

```cpp
#include <algorithm>

const int N = 3;
const int M = 3;
int arr[N][M] = {{3, 5, 1}, {1, 2, 9}, {4, 1, 7}};

bool cmp(const int (&a)[M], const int (&b)[M]) {
    return a[1] < b[1];
}

std::sort(arr, arr + N, cmp);
```

但这种写法依赖固定列数 `M`，旧版编译器兼容性和可读性都不如 `std::array`、结构体或索引数组。学习和竞赛中，建议根据是否允许改变数据结构来选择方案。

## 5. 多关键字排序

先按第 `0` 列升序；第 `0` 列相同时，按第 `1` 列降序：

```cpp
std::sort(arr.begin(), arr.end(),
    [](const std::vector<int>& a,
       const std::vector<int>& b) {
        if (a[0] != b[0]) {
            return a[0] < b[0];
        }
        return a[1] > b[1];
    });
```

逻辑模板：

```cpp
if (第一关键字不同) {
    return 第一关键字的排序规则;
}
return 第二关键字的排序规则;
```

## 6. 结构体通常更清晰

如果每行代表一个有意义的对象，推荐使用结构体：

```cpp
struct Student {
    int id;
    int score;
    std::string name;
};
```

然后按字段排序：

```cpp
std::sort(students.begin(), students.end(),
    [](const Student& a, const Student& b) {
        return a.score > b.score;
    });
```

`a.score` 比 `a[1]` 更容易理解，也不容易记错列号。

## 输入二维数据后按列排序

`std::cin >>` 会按照空白字符读取整数，换行和空格都可以作为分隔。先输入行数 `n`、列数 `m`，再输入 `n * m` 个元素，最后输入要排序的列号 `k`。`std::cin` 来自 `<iostream>`。

### `vector<vector<int>>` 输入

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    int n, m, k;                         // n 是行数，m 是列数，k 是排序列号
    std::cin >> n >> m;                   // 读取矩阵的行数和列数

    std::vector<std::vector<int> > arr(   // 创建 n 行、每行 m 个整数的二维容器
        n, std::vector<int>(m));

    for (int i = 0; i < n; ++i) {         // 逐行读取矩阵
        for (int j = 0; j < m; ++j) {     // 逐列读取当前行
            std::cin >> arr[i][j];       // 保存第 i 行第 j 列的输入值
        }
    }

    std::cin >> k;                        // 读取排序列号，列号从 0 开始

    std::sort(arr.begin(), arr.end(),     // 重新排列所有行
        [k](const std::vector<int>& a,    // a 是待比较的第一行
            const std::vector<int>& b) {  // b 是待比较的第二行
            return a[k] < b[k];           // 第 k 列较小的行排在前面
        });

    for (int i = 0; i < n; ++i) {         // 按排序后的顺序遍历每一行
        for (int j = 0; j < m; ++j) {     // 遍历当前行的每一列
            std::cout << arr[i][j] << ' '; // 输出当前元素
        }
        std::cout << '\n';               // 换行，开始输出下一行
    }
}
```

输入：

```text
3 3
3 5 1
1 2 9
4 1 7
1
```

含义：

- `3 3`：3 行 3 列。
- 接下来输入 9 个整数。
- 最后的 `1`：按第 1 列排序，也就是第二列。

输出：

```text
4 1 7
1 2 9
3 5 1
```

### 原生二维数组输入

```cpp
#include <algorithm>
#include <iostream>

int main() {
    const int N = 3;                      // 固定行数
    const int M = 3;                      // 固定列数
    int arr[N][M];                        // 保存原始二维数组

    for (int i = 0; i < N; ++i) {         // 逐行读取
        for (int j = 0; j < M; ++j) {     // 逐列读取
            std::cin >> arr[i][j];       // 保存当前元素
        }
    }

    int k;                                // 保存排序列号
    std::cin >> k;                        // 读取排序列号

    int index[N] = {0, 1, 2};             // 保存每一行原来的编号

    std::sort(index, index + N,            // 只排序行编号，不移动原始数组
        [&arr, k](int i, int j) {          // 比较原数组中的第 i 行和第 j 行
            return arr[i][k] < arr[j][k];  // 按第 k 列升序比较
        });

    for (int p = 0; p < N; ++p) {          // 按排序后的编号顺序输出
        int row = index[p];                // 取出当前应该输出的原始行号
        for (int col = 0; col < M; ++col) { // 输出该行的所有列
            std::cout << arr[row][col] << ' '; // 输出当前元素
        }
        std::cout << '\n';                // 换行
    }
}
```

这里输入完成后只排序 `index`，原数组 `arr` 的内容不改变。

## 输入时的检查

1. 列号从 `0` 开始，必须满足 `0 <= k < m`。
2. `vector<vector<int>>` 按列排序前，所有行都必须存在第 `k` 列。
3. 输入行数和列数后，必须读取完整的 `n * m` 个数据。
4. 输入可以写在同一行，也可以分成多行，`std::cin` 都能按空白分隔读取。

## 选择方法

| 数据形式 | 推荐方案 |
| --- | --- |
| 行长不固定 | `vector<vector<T>>` + 比较器 |
| 行长固定 | `vector<array<T, M>>` |
| 每行代表对象 | `vector<Struct>` |
| 原生二维数组且不改原数据 | 行索引间接排序 |
| 原生二维数组且允许直接交换行 | 数组引用比较器，注意编译器兼容性 |

## 常见误区

1. `std::sort` 排的是行，比较器中才访问某一列；不能把 `a[k]` 当作排序区间的元素。
2. `vector<vector<int>>` 的行长度可能不同，访问 `a[k]` 前要确认下标有效。
3. 排序列与排序行是两个不同任务；按列排序通常意味着重新排列整行。
4. 用 `std::sort(&a[0][0], ...)` 会把所有元素展平排序，不能实现“按某列重新排列整行”。
5. `std::sort` 不保证相等元素保持原顺序；有此要求时使用 `std::stable_sort` 或显式写第二关键字。
6. lambda、`std::array` 的列表初始化依赖较新的 C++ 标准；旧版编译器可改用普通比较函数和 `vector` 的 `push_back`。

## 关联

二维数组的 sort 排序 · 结构体排序：比较器与多关键字排序 · std sort：区间、比较器与头文件 · 数组：基本定义与语法

## 来源

- C++ 标准库 `<algorithm>`：`std::sort` 的区间与比较器接口
- cppreference：`std::sort` 要求随机访问迭代器和比较器
