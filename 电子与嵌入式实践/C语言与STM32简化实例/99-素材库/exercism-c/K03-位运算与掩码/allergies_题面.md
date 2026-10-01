---
title: allergies_题面
tags:
  - 素材
created: 2026-10-01
status: 外来素材（中文译写）
source: exercism/c（MIT 许可）
---

# 题面：过敏原

> 来源：exercism/c 仓库 `exercises/practice/allergies`（MIT 许可）。本文为中文译写，英文原件未保留。

给定一个过敏测试分数，判断这个人是否对某一项过敏，并列出他全部的过敏项。

一次过敏测试会产出一个数字分数，这个分数里包含了所有被测过敏原的信息。

被测的项目与对应数值：

- 鸡蛋 eggs (1)
- 花生 peanuts (2)
- 贝类 shellfish (4)
- 草莓 strawberries (8)
- 番茄 tomatoes (16)
- 巧克力 chocolate (32)
- 花粉 pollen (64)
- 猫 cats (128)

所以如果 Tom 对花生和巧克力过敏，他的分数就是 34（2 + 32）。

现在只给你这个分数 34，你的程序要能说出：

- Tom 是否对上面列出的某一项过敏
- Tom 全部的过敏项

## 关键限制

分数里可能包含**上面没列出**的过敏原（即 256、512、1024 等）。你的程序要忽略这些部分。

例如分数是 257 时，只应报告鸡蛋（1）这一项过敏。

## 官方接口

```c
bool is_allergic_to(allergen_t allergen, unsigned int score);
allergen_list_t get_allergens(unsigned int score);
```

枚举顺序：`ALLERGEN_EGGS = 0`、`ALLERGEN_PEANUTS = 1`、……、`ALLERGEN_CATS = 7`、`ALLERGEN_COUNT = 8`。
