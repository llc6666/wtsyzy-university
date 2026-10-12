---
title: STM32 输入捕获：用定时器给边沿打时间戳
status: public
created: 2026-10-12
updated: 2026-10-12
tags: [STM32, 定时器, 输入捕获, PWM, SPL]
evidence: 概念整理；来源代码与官方资料已回读；本轮未编译、未下载、未上板
---

# STM32 输入捕获：用定时器给边沿打时间戳

## 适用场景

当输入信号是周期脉冲、测速脉冲、回波脉冲或需要测量高电平持续时间时，GPIO 只能告诉你当前电平，EXTI 只能告诉你发生了边沿，定时器输入捕获还可以记录边沿发生时的计数值。

核心关系：

~~~text
GPIO 判断电平
EXTI 报告边沿事件
TIM 输入捕获给边沿打时间戳
ADC 把模拟电压转换成数字量
~~~

本页采用 STM32F1 标准外设库的概念和函数名说明。不同芯片系列、定时器实例、库层和引脚映射需要重新查对应手册，不能直接照搬示例中的数值和引脚。

> 来源为 STM32 定时器学习笔记、标准外设库接口说明和 ST 官方资料整理；本轮未编译、未下载、未上板验证。

## 输入捕获到底保存什么

定时器启动后，计数器 CNT 按选定频率递增。输入通道检测到配置的有效边沿时，会把此刻的 CNT 锁存到对应的 TIMx_CCRx。捕获事件会置位 CCxIF，若处理不及时而下一次捕获覆盖了上一次尚未处理的数据，还可能置位 CCxOF 过捕获标志。

因此，TIM_GetCapture1() 读到的是“计数格数”，不是自动带单位的微秒或秒。时间单位取决于计数器的实际频率。

## 初始化按六层连接

不要把输入捕获理解为只填一个结构体。按下面顺序逐层核对：

1. **打开时钟**：打开定时器所在 APB 总线的 TIM 外设时钟和输入 GPIO 时钟。使用引脚重映射时，还要打开 AFIO 并配置映射。
2. **配置输入引脚**：输入捕获引脚使用输入模式，常见选择包括浮空、上拉或下拉。不要把 PWM 输出的 GPIO_Mode_AF_PP 直接搬过来。
3. **配置计数时基**：设置 PSC、ARR、计数方向和时钟分频，让 CNT 以已知速度运行。
4. **配置捕获通道**：填写 TIM_ICInitTypeDef，设置通道、有效边沿、输入映射、捕获预分频和数字滤波，然后调用 TIM_ICInit()。
5. **选择读取路径**：轮询使用 TIM_GetFlagStatus()；中断使用 TIM_ITConfig() 并在 IRQ 函数中读取捕获值；高频场景另行评估 DMA。
6. **启动计数器**：调用 TIM_Cmd(TIMx, ENABLE) 才开始计数。启动计数器不等于已经开启捕获中断。

计数频率与计数间隔：

~~~text
f_cnt = f_TIM / (PSC + 1)
t_tick = 1 / f_cnt
~~~

f_TIM 必须按 APB 分频规则核对。STM32F1 定时器时钟可能与对应 APB 时钟存在倍频关系，不能机械地把系统时钟或 PCLK 直接代入。

## TIM_ICInitTypeDef 的五个关键成员

| 成员 | 作用 | 常见入门取值 |
| --- | --- | --- |
| TIM_Channel | 选择捕获通道 | TIM_Channel_1 |
| TIM_ICPolarity | 选择有效边沿 | 上升沿或下降沿 |
| TIM_ICSelection | 选择输入如何连接到通道 | TIM_ICSelection_DirectTI |
| TIM_ICPrescaler | 每几个有效边沿保存一次 | TIM_ICPSC_DIV1 |
| TIM_ICFilter | 对输入进行数字滤波 | 从 0x00 起按噪声调整 |

TIM_ICPrescaler 和时基的 TIM_Prescaler 不是同一个概念：前者改变捕获事件的分频，后者改变 CNT 的计数速度。DirectTI 表示输入直接连接到对应捕获通道，交叉输入和内部触发线属于其他连接方式，使用前应结合具体芯片手册确认。

## 普通输入捕获与 PWM 输入模式

### 普通输入捕获

计数器自由运行。每个配置的边沿把当前 CNT 保存到一个 CCR，适合测量连续同方向边沿之间的周期、单个脉冲的到达时间、超声波回波的高电平持续时间，以及测速脉冲的频率。

### PWM 输入模式

同一个输入信号使用两个捕获通道：一个保存周期计数，另一个保存高电平时间。常见关系是：

~~~text
period_ticks = CCR_period
high_ticks   = CCR_high
duty         = high_ticks / period_ticks
frequency    = f_cnt / period_ticks
~~~

SPL 的 TIM_PWMIConfig() 可以帮助配置成配对通道，之后还要按芯片和库接口选择输入触发源、从模式和启动方式。PWM 输入模式常配合从模式复位，使每个触发边沿重新开始计数，因此该定时器不能再被当成完全自由运行的普通时基使用。

## 常用函数按任务记忆

| 函数 | 任务 |
| --- | --- |
| TIM_TimeBaseStructInit() | 填充时基结构体默认值 |
| TIM_TimeBaseInit() | 写入 PSC、ARR 和计数方向 |
| TIM_ICStructInit() | 填充输入捕获结构体默认值 |
| TIM_ICInit() | 初始化普通输入捕获通道 |
| TIM_PWMIConfig() | 配置 PWM 输入模式的配对通道 |
| TIM_GetCapture1() 到 TIM_GetCapture4() | 读取对应 CCR 的捕获值 |
| TIM_GetCounter() / TIM_SetCounter() | 读取或设置 CNT |
| TIM_GetFlagStatus() / TIM_ClearFlag() | 轮询路径查询或清除状态标志 |
| TIM_ITConfig() | 开启或关闭捕获中断 |
| TIM_GetITStatus() / TIM_ClearITPendingBit() | 处理中断标志和挂起位 |
| TIM_SelectInputTrigger() | 选择输入触发源 |
| TIM_SelectSlaveMode() | 设置复位、门控或触发等从模式 |
| TIM_Cmd() | 启动或停止定时器 |

TIM_SetComparex() 属于输出比较/PWM 写入路径；普通输入捕获主要读取 TIM_GetCapturex()。

## 轮询和中断的结构

轮询动作顺序：

~~~text
查询对应 CCx 标志
→ 读取同一通道的 CCRx
→ 保存计数值或计算差值
→ 清除同一通道的标志
~~~

中断函数的动作顺序也是同一条链：确认 CCx 来源，读取对应捕获寄存器，只保存结果或置事件标志，最后清除对应挂起位。

不要检查 CC1 却读取 CCR2，也不要清除另一个通道的标志。中断服务函数中不要放延时、OLED 刷新或大量串口输出，否则高频信号可能在处理前再次到达并触发过捕获。

## 从计数差计算时间和频率

连续两个同方向边沿的捕获值记为 previous_capture 和 current_capture：

~~~text
delta_ticks = current_capture - previous_capture
time        = delta_ticks × t_tick
frequency   = f_cnt / delta_ticks
~~~

当计数器按完整 16 位范围运行时，合适的无符号差值可以自然处理一次回绕；如果 ARR 使用了其他值，则必须按实际模数处理。还要判断两次边沿之间是否可能跨越多圈，不能把任意长时间间隔都简化成一次 16 位相减。

示例只用于说明单位换算：若 f_TIM = 72 MHz、PSC = 71，则 f_cnt = 1 MHz，一个计数格约为 1 微秒。两个同方向边沿的计数差为 1500 时，周期约为 1.5 ms，频率约为 666.7 Hz。实际工程仍需重新核对定时器时钟、计数范围、边沿配置和信号质量。

## 常见错误与修正

| 错误 | 为什么错 | 修正 |
| --- | --- | --- |
| 把 TIMxCLK 当成固定 72 MHz | 定时器时钟取决于芯片和 APB 配置 | 先从时钟树和参考手册确认 |
| 输入脚沿用 PWM 的复用推挽输出 | 输入捕获是输入通道，电气模式不同 | 按外部电路选择输入模式 |
| 只调用 TIM_Cmd() 就等待中断 | 启动计数不等于开启捕获中断和 NVIC | 分开核对计数、捕获、IRQ 三层 |
| 把 TIM_ICPrescaler 当成时基分频 | 一个分频边沿，一个分频计数时钟 | 分别记录两个参数的作用 |
| 把捕获值直接当微秒 | CCR 保存的是计数格数 | 先算 f_cnt 和 t_tick |
| 忽略回绕或过捕获 | 差值可能不代表真实时间，CCR 可能已被覆盖 | 设计计数范围、回绕和溢出记录 |
| ISR 中打印或刷新显示 | 处理时间过长会错过后续捕获 | ISR 只保存数据或置标志 |
| 光敏 AO 使用输入捕获 | AO 是模拟电压，不是周期边沿 | AO 用 ADC，DO 按电平或边沿需求用 GPIO/EXTI |

## 读者自查清单

~~~text
[ ] 已确认芯片型号、定时器实例、通道和 GPIO 映射
[ ] 已确认 APB 分频与实际 TIMxCLK
[ ] 已区分 TIM_Prescaler 与 TIM_ICPrescaler
[ ] 已写出 CNT 的计数单位和 ARR 的回绕范围
[ ] 已决定使用轮询、中断还是 DMA
[ ] 检查、读取、清除使用的是同一个 CCx 通道
[ ] 已考虑输入悬空、噪声、回绕和过捕获
[ ] 已区分普通输入捕获与 PWM 输入模式
[ ] 已把来源记录、概念解释和实际板上结果分开
~~~

## 不适用范围

- 输入捕获不能替代 ADC，也不能把普通电平直接变成模拟电压。
- 仅有一篇概念笔记不能证明某个定时器、引脚映射或工程配置已经在具体开发板上可用。
- 频率很高、边沿很密或需要长期采样时，应单独设计 DMA、缓冲、过捕获和数据处理方案。
- 共享计数、读取后清零、跨执行流复合数据涉及原子性与交接协议；volatile 不能替代完整的并发设计。
- 本页没有代替读者编译、下载、接线、示波器测量或上板实验。

## 来源与复核

- 本地 STM32F1 定时器学习笔记：输入捕获初始化、TIM_ICInitTypeDef、普通捕获、PWM 输入、轮询/中断读取、测量公式和错误边界。
- 本地 SPL 接口：TIM_ICInitTypeDef、TIM_ICInit、TIM_PWMIConfig、TIM_ICStructInit、TIM_GetCapturex、标志和中断函数。
- [ST RM0008 Reference Manual](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)：输入捕获寄存器锁存、CCxIF 和 CCxOF 说明。
- [ST AN4776 General-purpose timer cookbook](https://www.st.com/resource/en/application_note/dm00236305-general-purpose-timer-cookbook-for-stm32-microcontrollers-stmicroelectronics.pdf)：输入捕获时序和周期测量示例。
- [GCC Volatiles](https://gcc.gnu.org/onlinedocs/gcc/Volatiles.html)：共享变量可见性与能力边界。
- 本页是概念整理，来源已有状态为“未在 Keil 工程编译、未下载、未上板验证”；本轮仅完成笔记整理和公开边界处理。

## 相关页面

- [电子与嵌入式实践](README.md)
- [非阻塞主循环：到期检查、按键消抖与任务验收](非阻塞主循环：到期检查、按键消抖与任务验收.md)
- [STM32 中断优先级：抢占、子优先级与验收](STM32中断优先级：抢占、子优先级与验收.md)
- [旋转编码器：A/B 相位与 STM32 计数](旋转编码器：A_B相位与STM32计数.md)
- [嵌入式输入排障：区分电平、状态与事件](输入排障：区分电平、状态与事件.md)

