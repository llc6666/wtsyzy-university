---
title: if、else 与条件分支：语法和用法
tags:
  - c-plus-plus
  - control-flow
  - basic-syntax
created: 2026-08-11
---

# if、else 与条件分支：语法和用法

## 一句话结论

`if` 用来根据条件选择是否执行一段代码：条件为真执行 `if` 分支，条件为假则跳过它；有 `else` 时，条件为假执行 `else` 分支。`else if` 用于按顺序检查多个互斥条件。

## 最基本的语法

```cpp
if (条件) {
    // 条件为 true 时执行
}
```

例如：

```cpp
int score = 80;

if (score >= 60) {
    std::cout << "及格\n";
}
```

`score >= 60` 的结果是 `true` 或 `false`。`if` 只在结果为 `true` 时执行花括号中的代码。

## `if ... else`

```cpp
if (条件) {
    // 条件为 true 时执行
} else {
    // 条件为 false 时执行
}
```

```cpp
int age = 17;

if (age >= 18) {
    std::cout << "成年人\n";
} else {
    std::cout << "未成年人\n";
}
```

一次 `if ... else` 中，两个分支只会执行其中一个。

## `if ... else if ... else`

当情况超过两种时，按从上到下的顺序判断：

```cpp
int score = 85;

if (score >= 90) {
    std::cout << "优秀\n";
} else if (score >= 60) {
    std::cout << "及格\n";
} else {
    std::cout << "不及格\n";
}
```

程序先检查 `score >= 90`。结果为假后，才检查 `score >= 60`。某个分支一旦执行，后续的 `else if` 和 `else` 都不会再检查。

条件顺序会影响结果。下面的顺序错误：

```cpp
if (score >= 60) {
    std::cout << "及格\n";
} else if (score >= 90) {
    std::cout << "优秀\n"; // 永远到不了这里
}
```

因为分数大于等于 90 时，同样先满足 `score >= 60`。

## 常用条件运算符

| 写法 | 含义 | 示例 |
| --- | --- | --- |
| `==` | 是否相等 | `score == 100` |
| `!=` | 是否不相等 | `score != 0` |
| `>` | 是否大于 | `age > 18` |
| `<` | 是否小于 | `count < 10` |
| `>=` | 是否大于或等于 | `score >= 60` |
| `<=` | 是否小于或等于 | `size <= 0` |
| `&&` | 两边都为真 | `age >= 18 && age < 60` |
| `||` | 至少一边为真 | `month == 1 || month == 12` |
| `!` | 取反 | `!isEmpty` |

### `&&`：并且

```cpp
if (age >= 18 && age < 60) {
    std::cout << "满足两个条件\n";
}
```

只有左右两个条件都为真，整体才为真。

| 左边 | 右边 | `&&` 结果 |
| --- | --- | --- |
| 真 | 真 | 真 |
| 真 | 假 | 假 |
| 假 | 真 | 假 |
| 假 | 假 | 假 |

`&&` 具有短路行为：左侧为假时，右侧不会执行。这使下面的链表判断安全：

```cpp
if (p != nullptr && p->data == target) {
    // 只有 p 不为空时，才会访问 p->data
}
```

若把顺序写反：

```cpp
// 错误：p 为空时，仍会先访问 p->data
if (p->data == target && p != nullptr) {
}
```

### `||`：或者

```cpp
if (month == 1 || month == 12) {
    std::cout << "冬季月份\n";
}
```

左右至少一个条件为真，整体就为真。

| 左边 | 右边 | `||` 结果 |
| --- | --- | --- |
| 真 | 真 | 真 |
| 真 | 假 | 真 |
| 假 | 真 | 真 |
| 假 | 假 | 假 |

`||` 也具有短路行为：左侧为真时，右侧不会执行。例如输入校验可以写：

```cpp
if (number < 1 || number > 10) {
    std::cout << "输入不在 1 到 10 的范围内\n";
}
```

`&&` 和 `||` 是逻辑运算符，不等于按位运算符 `&` 和 `|`。条件判断中应使用双符号。

## 花括号必须成为习惯

C++ 允许分支只控制紧随其后的**一条语句**：

```cpp
if (score >= 60)
    std::cout << "及格\n";
```

但初学阶段应始终写花括号：

```cpp
if (score >= 60) {
    std::cout << "及格\n";
}
```

否则后续新增语句时容易产生错误：

```cpp
if (score >= 60)
    std::cout << "及格\n";
    std::cout << "本行无条件执行\n";
```

第二行输出不属于 `if` 分支，因为没有花括号。

## 最常见错误：把 `==` 写成 `=`

```cpp
int score = 60;

if (score = 0) { // 错误意图：这是赋值，不是比较
    std::cout << "零分\n";
}
```

这里先把 `0` 赋给 `score`，赋值表达式结果为 `0`，转换为布尔值后是 `false`。正确写法：

```cpp
if (score == 0) {
    std::cout << "零分\n";
}
```

## `bool` 可以赋值 `0` 或 `1` 吗

可以：

```cpp
bool a = 0; // false
bool b = 1; // true
```

更一般地，赋给 `bool` 的数值会转换：`0` 变为 `false`，任何非零数值都变为 `true`。

```cpp
bool a = 0;   // false
bool b = 1;   // true
bool c = 42;  // true
bool d = -3;  // true
```

但表达“真假状态”时，应直接写 `true` 和 `false`：

```cpp
bool isFound = false;
isFound = true;
```

`0` 和 `1` 适合解释底层转换或兼容旧代码；用它们表达布尔语义会降低可读性。

反向转换也成立：当 `bool` 参与整数运算或赋给整数时，`false` 转为 `0`，`true` 转为 `1`。

```cpp
bool ok = true;
int value = ok; // 1
```

注意：`bool b = 2;` 的结果是 `true`，不会保留为“值 2”。`bool` 的语义只有真和假两种。
## 条件不一定要直接写 `true` 或 `false`

`if` 中的表达式会转换为布尔值。常见规则：数值 `0` 为假，非零数值为真；空指针 `nullptr` 为假，非空指针为真。

```cpp
int count = 0;
if (count) {
    // 不执行
}

Node* p = nullptr;
if (p) {
    // 不执行
}
```

语法允许这样写，但表达意图通常不如以下写法清楚：

```cpp
if (count != 0) {
}

if (p != nullptr) {
}
```

## `else` 与哪个 `if` 配对

没有花括号的嵌套条件中，`else` 会匹配离它最近且尚未匹配 `else` 的 `if`。

```cpp
if (a)
    if (b)
        action1();
    else
        action2();
```

这里的 `else` 属于 `if (b)`，不是 `if (a)`。嵌套分支必须使用花括号明确结构：

```cpp
if (a) {
    if (b) {
        action1();
    } else {
        action2();
    }
}
```

## 链表中的实际用法

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

这里的 `if` 只回答一个问题：当前节点的 `data` 是否等于要找的 `target`。若相等，立即返回；若不等，继续遍历下一个节点。

## 使用规则

1. `if` 条件必须表达一个可判断真假的问题。
2. 多个互斥范围从严格、范围小的条件写到宽松、范围大的条件。
3. 始终写花括号，即使当前分支只有一行。
4. 比较相等用 `==`，赋值用 `=`，两者不能混淆。
5. 指针成员访问前，先确认指针不为 `nullptr`；可使用 `&&` 的短路行为。
6. `else` 用于剩余情况；若剩余情况无需处理，可以不写 `else`。

## 关联

C++ 学习笔记 · List 的 size 与 find：链表遍历指针的使用 · NULL 与 nullptr：空指针的现代写法 · 函数返回值：return 交回什么

## 来源

- C++ 工作草案，[stmt.if]：`if`、`else` 与最近 `if` 的匹配规则。<https://eel.is/c++draft/stmt.if>
- C++ 工作草案，[conv.bool]：数值和指针转换为 `bool` 的规则。<https://eel.is/c++draft/conv.bool>
- C++ 工作草案，[expr.log.and]：`&&` 的左到右求值与短路规则。<https://eel.is/c++draft/expr.log.and>
