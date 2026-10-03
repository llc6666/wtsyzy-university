---
title: S05 非阻塞时基与帧调度
tags:
  - embedded
  - systick
  - state-machine
  - timing
created: 2026-10-01
updated: 2026-10-03
status: PC 模拟程序已编译运行
---

# S05 非阻塞时基与帧调度

## 一句话结论

**主循环的正确形态是：一个全局毫秒时基 + 每个任务记一个「下次动作时间」，到点做事、没到就让过。** 阻塞延时（`Delay_ms`）期间 CPU 出不去，排在后面的任务全部停摆。

## 实测（PC 模拟）

文件：[tick-frame-demo.c](tick-frame-demo.c)。红绿灯（红 3s → 绿 2s → 黄 0.5s）+ 每 1000ms 一次的心跳任务，同一时基两种写法：

```bash
gcc -std=c17 -Wall -Wextra -Wpedantic tick-frame-demo.c -o tick-frame-demo.exe
```

```text
== 1. 阻塞式 ==
[t=    0ms] 红灯亮
[t= 3000ms] 绿灯亮
[t= 5000ms] 黄灯亮
[t= 5500ms] 一轮结束。心跳任务排在 Delay 后面，6000ms 里 0 次（本应 6 次）

== 2. 非阻塞 ==
[t=    0ms] 红灯亮
[t= 1000ms] 心跳 #1
[t= 2000ms] 心跳 #2
[t= 3000ms] 心跳 #3
[t= 3000ms] 绿灯亮
[t= 4000ms] 心跳 #4
[t= 5000ms] 心跳 #5
[t= 5000ms] 黄灯亮
[t= 5500ms] 红灯亮
[t= 6000ms] 心跳 #6
6000ms：心跳 6 次一次不落，灯按时切换，两件事都没耽误
```

回绕段（`uint32_t` 计满归零，等 20ms）：

```text
起始 now=0xFFFFFFF0，目标 target=0x00000004
  now=0xFFFFFFF0  有符号差值 -20  -> 还没到
  now=0xFFFFFFFF  有符号差值  -5  -> 还没到
  now=0x00000004  有符号差值   0  -> 到点
错误写法 now >= target：0xFFFFFFF0 >= 4 为真 -> 提前 20ms 误触发
```

## 一、阻塞 Delay 的真实代价

课程示例 `Delay_ms` 是让 SysTick 计数、CPU 空轮询。单任务没问题，**第二个任务一加就露馅**：心跳排在 `Delay` 后面，6000ms 一次都轮不到（上面第 1 段实测）。课堂例程看着能用，是因为例程里只有一个任务。

## 二、非阻塞骨架

```c
volatile uint32_t g_ms = 0;          /* SysTick 每 1ms +1，原型 SysTick_Config */

while (1) {
    if (!not_yet(g_ms, beat_at))   { 心跳();   beat_at   += 1000; }
    if (!not_yet(g_ms, switch_at)) { 切灯();   switch_at += duration; }
}
```

要点三个：**到点判断与时间推进分离**（PC 模拟在本轮末尾推进时间，真实硬件由中断计时）；**每个任务一张「时刻表」**（`beat_at`/`switch_at` 独立保存，但仍共享 CPU，一个任务耗时过长会延误另一个）；**状态机只管「现在是什么灯」，时长是数据不是代码**（`light_duration[]` 表驱动）。

## 三、回绕：为什么差值要转有符号

`uint32_t` 毫秒计数约 49.7 天回绕一次。回绕窗口里 `now` 是巨大值、`target` 是小值：

- `now >= target`：恒真 → 提前触发（实测提前 20ms 误判）
- `(int32_t)(now - target) < 0`：本例在所测 GCC 平台与短时间跨度下符合预期；不能无条件推广到所有平台和时间跨度。

这里把超出 `int32_t` 范围的无符号值转为有符号值，在 C17 下结果由实现规定；截止时刻比较还要求距离小于半个计数周期。新任务优先参考[经过时间与消抖验收](../../../非阻塞主循环：到期检查、按键消抖与任务验收.md)中的无符号经过时间写法，并满足真实经过 tick 少于完整回绕周期等条件。标准依据见 [WG14 N1570 §6.3.1.3](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf)。

这条写法是 K02（有符号/无符号）与 K10（时基变量 volatile）的合题，工程原型里就是 `OLED_WaitUntil` 那一行。

## 四、帧动画是同一条规律

「每 100ms 渲染一帧」= 心跳任务的周期版：`frame_at += 100` 到点画一帧（配合 S04 的攒帧刷新）。红绿灯、心跳、动画帧、软件看门狗喂狗，全部是**时基 + 时刻表**这一种结构。

## 按键与光敏任务的进阶验收

新增独立 C17 模拟：[nonblocking-button-light-demo.c](nonblocking-button-light-demo.c)。说明与 PowerShell 运行命令见[到期检查、按键消抖与任务验收](../../../非阻塞主循环：到期检查、按键消抖与任务验收.md)。它增加稳定窗口、仅按下触发、错过采样的处理和回绕测试；仍不代表板上实测。

## 用到的 C 知识点

- [K04 控制流与循环](../../C知识点/K04-控制流与循环/README.md)：状态机、表驱动时长
- [K10 volatile 与编译器优化](../../C知识点/K10-volatile与编译器优化/README.md)：`g_ms` 由中断改、主循环读
- [K02 类型与固定宽度整数](../../C知识点/K02-类型与固定宽度整数/README.md)：回绕、有符号差值判「还没到」

## SPL 对照与工程原型

见 [嵌入式实例.md](嵌入式实例.md)。原型：`本地 OLED 工程原型`（`SysTick_Config` + `OLED_WaitUntil`，Keil 编译通过）。
