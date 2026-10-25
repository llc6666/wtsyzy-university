---
title: STM32F1 引脚复用与 AFIO 重映射：配置核对卡
status: public
updated: 2026-10-25
tags:
  - stm32f1
  - gpio
  - afio
  - pin-remap
---

# STM32F1 引脚复用与 AFIO 重映射：配置核对卡

## 这页解决什么问题

代码能编译，外设却没有信号，常见原因是把“芯片支持的功能”“当前封装引出的引脚”“工程实际配置的引脚”混成了一件事。

本文面向 STM32F1 系列中使用 AFIO 和 SPL 的学习场景，重点是建立查表和排查顺序。示例以 STM32F103x8/xB、LQFP48 和 STM32F10x Standard Peripheral Library 为背景，不能直接套到其他系列、封装或 HAL 工程。

## 一、先建立三层事实

| 层 | 要回答的问题 | 证据 |
| --- | --- | --- |
| 芯片与封装 | 这个型号和封装允许哪些默认功能、重映射功能？ | 芯片数据手册的引脚定义表 |
| 板卡与电路 | 该脚是否引出，是否被晶振、调试接口、LED 或外部器件占用？ | 板卡原理图、丝印与实际接线 |
| 工程配置 | GPIO 模式、外设时钟、AFIO 映射和外设通道是否一致？ | 初始化代码、配置寄存器与工程文件 |

先完成三层核对，再写“引脚已验证”。数据手册里的 alternate function 表描述的是芯片能力，不等于某块开发板的接线表。

## 二、默认复用、GPIO 模式和重映射各管什么

- **GPIO 模式**决定引脚的电气行为，例如普通输入、复用推挽输出或复用开漏输出。
- **默认复用**决定外设在复位后的常用引脚组合，例如 USART1 常用 PA9/PA10，SPI1 常用 PA4 至 PA7。
- **AFIO 重映射**把芯片规定的另一组引脚接给外设，例如把 USART1 换到 PB6/PB7。它写的是外设线路选择，不是把任意 GPIO 变成任意外设。
- **EXTI 端口选择**处理外部中断线与端口的对应关系。它和外设重映射有关联，但不能拿 `GPIO_Mode_AF_PP` 或 `GPIO_PinRemapConfig()` 代替。

同一组引脚可能有多个候选功能。PB6/PB7 既可能用于 I2C1，也可能用于重映射后的 USART1；同一时间只能按当前设计选择其中一种用途。

## 三、常见组合只作为查表入口

| 外设 | 常见默认引脚 | 学习时先检查 |
| --- | --- | --- |
| USART1 | PA9/PA10 | TX 是否复用推挽，RX 是否按外部电路配置输入；重映射时是否改用 PB6/PB7 |
| USART2 | PA2/PA3 | 外设时钟、GPIOA 时钟和 TX/RX 的方向 |
| I2C1 | PB6/PB7 | 开漏模式与外部上拉；若改用 PB8/PB9，确认芯片和封装支持 |
| SPI1 | PA4/PA5/PA6/PA7 | NSS、SCK、MISO、MOSI 的角色和是否与调试脚冲突 |
| TIM2 | PA0/PA1/PA2/PA3 | 通道编号与定时器重映射组合，不能只看引脚编号 |
| TIM3 | PA6/PA7/PB0/PB1 | PWM 通道、GPIO 复用模式和外部负载 |

表格只用于形成候选清单。最终以对应型号的数据手册和板卡原理图为准。

## 四、EXTI 线是“按编号选一个端口”

STM32F1 的 EXTI 线按引脚编号组织。PA14、PB14 和 PC14 都对应 14 号线，但同一时刻只能把这条线接到一个端口。AFIO 的 `EXTICR` 字段保存的是端口选择值：

```c
/* PB14 -> EXTI14，参数是端口和线号，不是 GPIO 位掩码 */
GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
```

这里的 `GPIO_PinSource14` 表示“14 号线”。它不是 `GPIO_Pin_14` 那种位掩码，也不能把多个 `PinSource` 用 `|` 合并后传入一次调用。需要配置多条线时，分别调用并分别核对字段。

以 EXTI0 到 EXTI15 为例，每条线在 `EXTICR` 中占 4 位，字段值代表端口。于是排查顺序是：

1. 确认引脚编号与 EXTI 线号相同。
2. 确认 AFIO 时钟已开启。
3. 确认端口选择指向实际使用的 GPIO 端口。
4. 再检查 EXTI 触发沿、NVIC 通道和中断函数名。

EXTI 端口选择解决“哪一个端口的同号引脚进入这条线”，触发沿解决“什么变化产生请求”，NVIC 解决“CPU 是否接受请求”。三者职责不能混写。

## 五、外设重映射的最小配置思路

以 USART1 改到 PB6/PB7 为例，先在纸上写出三张表：

```text
外设：USART1
默认：PA9 = TX，PA10 = RX
重映射：PB6 = TX，PB7 = RX
冲突：PB6/PB7 不能同时承担 I2C1
```

随后按这个顺序写工程配置：

1. 开启 GPIOB、USART1 和 AFIO 所在总线的时钟。
2. 按信号方向配置 TX 与 RX 的 GPIO 模式。
3. 调用对应的 `GPIO_PinRemapConfig()` 宏。
4. 初始化 USART 外设参数。
5. 对照芯片数据手册、板卡原理图和代码，逐项确认三个位置一致。

示意片段：

```c
RCC_APB2PeriphClockCmd(
    RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,
    ENABLE
);
GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
```

这段代码只表达“选择重映射线路”。它没有替代 TX/RX 的 GPIO 初始化、USART 参数配置、板卡接线检查和实际信号观测。

## 六、遇到“编译通过但没有信号”时按表排查

| 现象 | 优先核对 |
| --- | --- |
| 串口 TX 没波形 | TX 是否接在默认或重映射后的正确脚；GPIO 是否为复用输出；USART 时钟是否开启 |
| 串口 RX 没反应 | RX 是否配置成输入；外部设备地线和电平是否匹配；工程是否仍使用默认映射 |
| I2C 总线被拉不高 | 是否使用开漏输出；上拉是否存在；PB6/PB7 是否被其他外设重映射占用 |
| EXTI 只响应某个端口 | `GPIO_EXTILineConfig()` 的端口和线号是否对应；是否误把线号当位掩码 |
| 改了 SPI1 重映射后调试失效 | PA15、PB3、PB4 是否与 JTAG/SWD 方案冲突；是否保留了需要的调试接口 |
| 代码和电路各写一组引脚 | 回到数据手册、原理图和初始化代码三层事实表，重新逐项对齐 |

任何“已验证”结论都要补齐型号、封装、工程配置、接线和观测方式。只有编译通过，不能证明引脚映射正确；只有芯片手册列出某功能，也不能证明当前板卡已接出该功能。

## 最小核对清单

- [ ] 型号和封装已写清。
- [ ] 默认功能、Remap 功能和封装可用性来自对应数据手册。
- [ ] 板卡原理图确认引脚确实引出，且没有被晶振、调试接口或板载器件占用。
- [ ] GPIO 端口时钟、外设时钟和需要的 AFIO 时钟已列出。
- [ ] TX/RX、开漏/推挽、输入上拉等电气模式与信号方向匹配。
- [ ] 外设重映射与 EXTI 端口选择没有混用。
- [ ] 代码、原理图、实际接线和观测记录一致后，才写发布结论。

## 官方来源与边界

- [STM32F103x8/STM32F103xB 数据手册](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf)：引脚定义、默认功能、重映射功能和封装边界。
- [RM0008 参考手册](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)：GPIO、AFIO、MAPR、EXTICR 和调试端口重映射。
- [STM32 标准外设库](https://www.st.com/en/embedded-software/stm32-standard-peripheral-libraries.html)：SPL 的官方入口，具体宏和函数仍要结合目标库版本查看。

本文只整理 STM32F1 的常见配置和排查顺序，没有覆盖全部型号、封装和重映射矩阵。来源示例未编译，本轮未编译、未运行、未下载、未烧录、未上板。
