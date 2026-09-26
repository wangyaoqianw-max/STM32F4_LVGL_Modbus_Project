# STM32F407VET6 CubeMX 外设与引脚配置基线

- 日期：2026-09-26
- 工作项：`PREPARATION`
- 目标工程：`03_Firmware/Application/STM32F407_APP`
- 目标 MCU：STM32F407VET6，LQFP100
- 用途：按功能模块给出 Application 母工程的 STM32CubeMX 外设、GPIO、DMA、NVIC 和时钟配置入口。
- 状态：静态配置基线；未进行本轮板级验证。
- 原则：已确认硬件连接直接配置；项目分配直接冻结；尚未确认的电气或时序参数明确保留为 `TO_VERIFY`。

> 本文档负责回答“CubeMX 中具体打开什么、选什么参数、使用哪些引脚”。业务协议、驱动 API、RTOS Task 设计和 LVGL/Modbus 上层实现不在本文展开。
>
> 若本文与较早的资源梳理文档存在状态差异，以“最新已确认硬件事实 + Project Owner 最新明确分配”为准；未验证事项不得从示例工程反推为板级事实。

---

## 1. 配置状态定义

| 状态 | 含义 |
| --- | --- |
| `CONFIRMED` | 已由开发板引脚表、原理图分析或现有项目硬件事实直接确认 |
| `PROJECT_DECISION` | Project Owner 已明确分配，作为本项目配置冻结 |
| `RECOMMENDED_BASELINE` | 为 Bring-up 提供的保守初始参数，可在板测后优化 |
| `TO_VERIFY` | 仍需原理图、器件手册或实机验证，当前不得写成硬件事实 |

---

## 2. 模块总览

| 模块 | CubeMX 外设/方式 | 引脚 | 当前状态 |
| --- | --- | --- | --- |
| SWD 调试 | SYS / Serial Wire | PA13 SWDIO, PA14 SWCLK | `CONFIRMED` |
| 系统时钟 | RCC / HSE + PLL | HSE 引脚由芯片固定；频率待核对 | `TO_VERIFY` |
| LSE | RCC / LSE | PC14 OSC32_IN, PC15 OSC32_OUT | 板载连接 `CONFIRMED`，当前功能不启用 |
| AT24C02 + DHT20 | GPIO 软件 I²C | PB6 SCL, PB7 SDA | `CONFIRMED + PROJECT_DECISION` |
| W25Q128 | SPI1 | PA5 SCK, PA6 MISO, PA7 MOSI, PC13 CS | `CONFIRMED` |
| OTA / YMODEM | USART1 | PA9 TX, PA10 RX | `PROJECT_DECISION` |
| HC-05 | USART2 | PA2 TX, PA3 RX | `PROJECT_DECISION` |
| RS485 / Modbus RTU | USART3 | PB10 TX, PB11 RX | `CONFIRMED + PROJECT_DECISION` |
| ILI9341 | FSMC 16-bit 8080 | PD/PE 见 §9 | `CONFIRMED` |
| LCD 背光 | GPIO | PA15 LCD_BLK | `CONFIRMED` |
| XPT2046 | GPIO 模拟 SPI | PE0/PE2/PE3/PE4, PD13 | `CONFIRMED` |
| FreeRTOS | FREERTOS / CMSIS-V2 | 无固定 GPIO | `PROJECT_DECISION` |
| HAL Tick | SYS / TIM2 | 内部资源 | `RECOMMENDED_BASELINE` |

当前不启用：CAN1、SDIO、USB OTG FS、板载按键业务、板载 LED 业务、ESP8266。

---

## 3. SYS：调试接口与 HAL Timebase

### 3.1 Debug

CubeMX：

`System Core -> SYS`

配置：

| 选项 | 设置 |
| --- | --- |
| Debug | `Serial Wire` |
| Timebase Source | FreeRTOS 启用后改为 `TIM2` |

引脚：

| 信号 | 引脚 |
| --- | --- |
| SWDIO | PA13 |
| SWCLK | PA14 |

注意：

- 不使用完整 JTAG。
- PA15 已用于 LCD 背光；选择 `Serial Wire` 可以避免 JTAG/JTDI 对 PA15 的占用。
- J-Link/RTT 调试不需要额外 UART。

### 3.2 HAL Timebase

启用 FreeRTOS 后：

`System Core -> SYS -> Timebase Source = TIM2`

建议：

- TIM2 IRQ 抢占优先级：`15`
- SysTick/PendSV 保持 RTOS 使用的最低优先级
- 不把业务代码放进 TIM2 HAL Tick ISR

---

## 4. RCC / Clock Configuration

### 4.1 当前不能冻结 HSE 频率

现有 `STM32F407_APP.ioc` 中存在 `HSE_VALUE=25 MHz`，但目前这不是已确认的 v5.5 板级事实。

当前已知：

- 引脚分配表确认 PC14/PC15 接板载 32.768 kHz 晶振；
- 现有公开的同系列旧版商家原理图曾使用 8 MHz HSE；
- 因此在核对本项目 v5.5 原理图或板载晶振丝印之前，不能直接沿用 25 MHz。

### 4.2 目标时钟树

无论最终 HSE 为 8 MHz 或 25 MHz，Application 目标保持：

| 时钟 | 目标 |
| --- | ---: |
| SYSCLK | 168 MHz |
| HCLK / AHB | 168 MHz |
| APB1 | 42 MHz |
| APB1 Timer Clock | 84 MHz |
| APB2 | 84 MHz |
| APB2 Timer Clock | 168 MHz |
| PLL48CLK | 48 MHz |

Clock Configuration：

- SYSCLK Source：`PLLCLK`
- AHB Prescaler：`/1`
- APB1 Prescaler：`/4`
- APB2 Prescaler：`/2`

### 4.3 若 v5.5 HSE = 8 MHz

RCC：

- HSE：`Crystal/Ceramic Resonator`
- PLL Source：`HSE`

PLL：

| 参数 | 值 |
| --- | ---: |
| PLLM | 8 |
| PLLN | 336 |
| PLLP | 2 |
| PLLQ | 7 |

计算：

`8 MHz / 8 * 336 / 2 = 168 MHz`

`8 MHz / 8 * 336 / 7 = 48 MHz`

### 4.4 若 v5.5 HSE = 25 MHz

PLL：

| 参数 | 值 |
| --- | ---: |
| PLLM | 25 |
| PLLN | 336 |
| PLLP | 2 |
| PLLQ | 7 |

计算：

`25 MHz / 25 * 336 / 2 = 168 MHz`

`25 MHz / 25 * 336 / 7 = 48 MHz`

### 4.5 LSE

虽然 PC14/PC15 已连接 32.768 kHz 晶振，但当前项目没有 RTC 需求。

当前基线：

`RCC -> LSE = Disable`

后续真正使用 RTC 时再启用，避免为了“板上有晶振”而无需求占用时钟域。

---

## 5. 软件 I²C：AT24C02 + DHT20

### 5.1 资源关系

```text
PB6 / SCL ──┬── AT24C02（板载）
            └── DHT20（外接杜邦线）

PB7 / SDA ──┬── AT24C02（板载）
            └── DHT20（外接杜邦线）
```

不启用 CubeMX 的 I2C1 外设。

### 5.2 PB6

Pinout：

`PB6 -> GPIO_Output`

GPIO Configuration：

| 选项 | 设置 |
| --- | --- |
| User Label | `I2C_SCL` |
| GPIO output level | `High` |
| GPIO mode | `Output Open Drain` |
| GPIO Pull-up/Pull-down | `No pull` |
| Maximum output speed | `Low` |

### 5.3 PB7

与 PB6 相同：

| 选项 | 设置 |
| --- | --- |
| User Label | `I2C_SDA` |
| GPIO output level | `High` |
| GPIO mode | `Output Open Drain` |
| GPIO Pull-up/Pull-down | `No pull` |
| Maximum output speed | `Low` |

### 5.4 约束

- SDA 驱动中需要按软件 I²C 实现切换“释放/读取”状态。
- 不依赖 MCU 内部弱上拉替代正式 I²C 上拉。
- PB6/PB7 的实际外部上拉阻值仍需从 v5.5 原理图核对。
- DHT20 外接模块若自带上拉，需要检查其与开发板现有上拉并联后的等效阻值。
- AT24C02 与 DHT20 必须通过同一软件 I²C 总线所有权/互斥机制访问。

---

## 6. W25Q128：SPI1

### 6.1 Pinout

| 信号 | 引脚 | CubeMX |
| --- | --- | --- |
| SCK | PA5 | SPI1_SCK |
| MISO | PA6 | SPI1_MISO |
| MOSI | PA7 | SPI1_MOSI |
| CS# | PC13 | GPIO_Output |

SPI1 使用 AF5。

### 6.2 SPI1 Parameter Settings

`Connectivity -> SPI1`

Mode：

`Full-Duplex Master`

建议初始参数：

| 参数 | 设置 |
| --- | --- |
| Frame Format | Motorola |
| Data Size | 8 Bits |
| First Bit | MSB First |
| Clock Polarity (CPOL) | Low |
| Clock Phase (CPHA) | 1 Edge |
| NSS | Software |
| Baud Rate Prescaler | `8` |
| TI Mode | Disable |
| CRC Calculation | Disable |

在 APB2 = 84 MHz 时：

`84 MHz / 8 = 10.5 MHz`

该速率作为 W25Q128 Bring-up 的保守初始频率；完成 JEDEC ID、读写、擦除和字体区只读验证后再评估提速。

### 6.3 PC13 / CS

`PC13 -> GPIO_Output`

| 参数 | 设置 |
| --- | --- |
| User Label | `W25Q128_CS` |
| Initial Level | `High` |
| Mode | Output Push Pull |
| Pull | No Pull |
| Speed | Low |

### 6.4 DMA / NVIC

第一版：

- SPI1 DMA：`Disable`
- SPI1 Global Interrupt：非必需，可不启用

原因：S03 初期以阻塞式 SPI 完成 JEDEC/Read/Write/Erase/Font Read 验证，避免提前占用 DMA2 Stream。

后续若 LVGL 字体读取证明确有吞吐需求，再单独评估 SPI1 DMA。

---

## 7. USART1：有线 OTA / YMODEM

### 7.1 Pinout

| 信号 | 引脚 |
| --- | --- |
| USART1_TX | PA9 |
| USART1_RX | PA10 |

开发板引脚表显示 USART1 与板载 USB-TTL 相连，因此 OTA PC 通道优先直接复用该链路。

### 7.2 USART1 Parameter Settings

`Connectivity -> USART1`

Mode：

`Asynchronous`

Bring-up 基线：

| 参数 | 设置 |
| --- | --- |
| Baud Rate | 115200 |
| Word Length | 8 Bits |
| Parity | None |
| Stop Bits | 1 |
| Data Direction | Receive and Transmit |
| Over Sampling | 16 Samples |
| Hardware Flow Control | None |

即 `115200-8-N-1`。

### 7.3 USART1 DMA

RX：

| 参数 | 设置 |
| --- | --- |
| Request | USART1_RX |
| DMA | DMA2 Stream2 |
| Channel | Channel 4 |
| Direction | Peripheral to Memory |
| Mode | Circular |
| Peripheral Increment | Disable |
| Memory Increment | Enable |
| Peripheral Data Width | Byte |
| Memory Data Width | Byte |
| Priority | Medium |
| FIFO | Disable |

TX：

| 参数 | 设置 |
| --- | --- |
| Request | USART1_TX |
| DMA | DMA2 Stream7 |
| Channel | Channel 4 |
| Direction | Memory to Peripheral |
| Mode | Normal |
| Peripheral Increment | Disable |
| Memory Increment | Enable |
| Data Width | Byte / Byte |
| Priority | Medium |
| FIFO | Disable |

### 7.4 USART1 NVIC

必须启用：

- `USART1 global interrupt`
- `DMA2 Stream2 global interrupt`
- `DMA2 Stream7 global interrupt`

建议：

- Preemption Priority：`5`
- Sub Priority：`0`

USART Global IRQ 需要保留，因为 DMA + IDLE 接收仍需要 USART IDLE 中断参与帧/批次边界通知。

---

## 8. USART2：HC-05

### 8.1 Pinout

开发板引脚表确认 PA2/PA3 当前板载功能为空闲，本项目分配：

| 信号 | 引脚 | 外接 |
| --- | --- | --- |
| USART2_TX | PA2 | 接 HC-05 RX |
| USART2_RX | PA3 | 接 HC-05 TX |

连接时必须共地。

### 8.2 USART2 Parameter Settings

`Connectivity -> USART2 -> Asynchronous`

UART 数据格式：

| 参数 | 设置 |
| --- | --- |
| Word Length | 8 Bits |
| Parity | None |
| Stop Bits | 1 |
| Direction | TX and RX |
| Over Sampling | 16 |
| Hardware Flow Control | None |

波特率：

`TO_VERIFY`

原因：HC-05 通信波特率可能被配置过，不能只根据“HC-05 型号”推断当前模块实际值。

推荐流程：

1. 初次板测先确认模块当前通信波特率；
2. 如需要统一工程配置，可再通过 AT 模式将模块固定到项目选定波特率；
3. 最终波特率进入 `config`，不要散落在 HC-05 Service 内。

若模块确认仍为常见出厂通信参数，可先尝试 `9600-8-N-1`，但该值当前不是冻结事实。

### 8.3 USART2 DMA

RX：

| 参数 | 设置 |
| --- | --- |
| Request | USART2_RX |
| DMA | DMA1 Stream5 |
| Channel | Channel 4 |
| Direction | Peripheral to Memory |
| Mode | Circular |
| Memory Increment | Enable |
| Data Width | Byte / Byte |
| Priority | Medium |
| FIFO | Disable |

TX：

| 参数 | 设置 |
| --- | --- |
| Request | USART2_TX |
| DMA | DMA1 Stream6 |
| Channel | Channel 4 |
| Direction | Memory to Peripheral |
| Mode | Normal |
| Memory Increment | Enable |
| Data Width | Byte / Byte |
| Priority | Medium |
| FIFO | Disable |

NVIC：

- USART2 global interrupt：Enable
- DMA1 Stream5：Enable
- DMA1 Stream6：Enable
- Priority：`5, 0`

---

## 9. USART3：RS485 / Modbus RTU

### 9.1 Pinout

| 信号 | 引脚 | 板级用途 |
| --- | --- | --- |
| USART3_TX | PB10 | RS485 |
| USART3_RX | PB11 | RS485 |

### 9.2 USART3 Parameter Settings

CubeMX：

`Connectivity -> USART3 -> Asynchronous`

不要选择：

- Single Wire Half Duplex
- IrDA
- SmartCard

原因：MCU 到 RS485 收发器仍然是普通 UART TX/RX；“半双工”发生在 RS485 A/B 物理总线上。

Bring-up 基线：

| 参数 | 设置 |
| --- | --- |
| Baud Rate | 115200（临时 Bring-up） |
| Word Length | 8 Bits |
| Parity | None |
| Stop Bits | 1 |
| Direction | TX and RX |
| Over Sampling | 16 |
| Hardware Flow Control | None |

最终 Modbus 波特率、Parity、Stop Bits 属于协议配置，正式寄存器/通信规范冻结时再确定；当前不要把 115200-8-N-1 当成发布接口。

### 9.3 RS485 DE/RE

当前开发板引脚分配表只列出 PB10/PB11，没有独立 MCU DE/RE GPIO。

因此当前 CubeMX 基线：

- 不额外分配 RS485_DE GPIO；
- 不在 UART 层假设存在 MCU 手动 DE/RE 控制；
- v5.5 原理图确认自动换向电路后，再将“自动方向控制”升级为 `CONFIRMED`。

在该确认完成前，本项状态为 `TO_VERIFY`。

### 9.4 USART3 DMA

RX：

| 参数 | 设置 |
| --- | --- |
| Request | USART3_RX |
| DMA | DMA1 Stream1 |
| Channel | Channel 4 |
| Direction | Peripheral to Memory |
| Mode | Circular |
| Memory Increment | Enable |
| Data Width | Byte / Byte |
| Priority | Medium |
| FIFO | Disable |

TX：

| 参数 | 设置 |
| --- | --- |
| Request | USART3_TX |
| DMA | DMA1 Stream3 |
| Channel | Channel 4 |
| Direction | Memory to Peripheral |
| Mode | Normal |
| Memory Increment | Enable |
| Data Width | Byte / Byte |
| Priority | Medium |
| FIFO | Disable |

NVIC：

- USART3 global interrupt：Enable
- DMA1 Stream1：Enable
- DMA1 Stream3：Enable
- Priority：`5, 0`

---

## 10. 三路 UART DMA 资源汇总

| 通道 | 业务 | RX | TX | Channel |
| --- | --- | --- | --- | --- |
| USART1 | OTA/YMODEM | DMA2 Stream2 | DMA2 Stream7 | Ch4 |
| USART2 | HC-05 | DMA1 Stream5 | DMA1 Stream6 | Ch4 |
| USART3 | RS485/Modbus | DMA1 Stream1 | DMA1 Stream3 | Ch4 |

该组合不存在 DMA Stream 冲突。

统一建议：

```text
RX DMA : Circular
TX DMA : Normal
Data Width : Byte
Peripheral Inc : Disable
Memory Inc : Enable
FIFO : Disable
```

后续移植旧 UART DMA + IDLE + RingBuffer 模块时，将其从单 UART 假设改造成静态多实例：

```text
USART1 -> uart_ota_ctx
USART2 -> uart_hc05_ctx
USART3 -> uart_modbus_ctx
```

不需要建立动态插件式 UART 注册中心。

运行时若使用 `HAL_UARTEx_ReceiveToIdle_DMA()` + Circular DMA，需要由 UART 通用模块统一处理当前位置差值；如不需要 Half Transfer 事件，可在驱动启动 RX DMA 后关闭 HT 中断以减少无意义回调。

---

## 11. ILI9341：FSMC 16-bit 8080

### 11.1 Pinout

| LCD | MCU | FSMC |
| --- | --- | --- |
| CS | PD7 | FSMC_NE1 |
| WR | PD5 | FSMC_NWE |
| RD | PD4 | FSMC_NOE |
| RS/DC | PD11 | FSMC_A16 |
| D0 | PD14 | FSMC_D0 |
| D1 | PD15 | FSMC_D1 |
| D2 | PD0 | FSMC_D2 |
| D3 | PD1 | FSMC_D3 |
| D4 | PE7 | FSMC_D4 |
| D5 | PE8 | FSMC_D5 |
| D6 | PE9 | FSMC_D6 |
| D7 | PE10 | FSMC_D7 |
| D8 | PE11 | FSMC_D8 |
| D9 | PE12 | FSMC_D9 |
| D10 | PE13 | FSMC_D10 |
| D11 | PE14 | FSMC_D11 |
| D12 | PE15 | FSMC_D12 |
| D13 | PD8 | FSMC_D13 |
| D14 | PD9 | FSMC_D14 |
| D15 | PD10 | FSMC_D15 |

上述 FSMC GPIO 使用 AF12。

### 11.2 FSMC 基本配置

CubeMX：

`Connectivity -> FSMC -> NOR/SRAM1`

建议：

| 参数 | 设置 |
| --- | --- |
| Bank | NOR/SRAM Bank1 |
| Chip Select | NE1 |
| Memory Type | SRAM |
| Data Address Mux | Disable |
| Memory Data Width | 16 bits |
| Burst Access Mode | Disable |
| Wait Signal | Disable |
| Write Operation | Enable |
| Extended Mode | Enable |
| Asynchronous Wait | Disable |
| Write Burst | Disable |

使用 SRAM 类型的原因是 ILI9341 的 8080 异步接口只需要 FSMC 产生地址、CS、RD、WR 和数据总线时序，不代表外接器件实际是 SRAM。

### 11.3 FSMC 初始时序

以下参数是 168 MHz HCLK 下用于 Bring-up 的保守基线，不是性能优化结果。

#### Read Timing

| 参数 | 初始值 |
| --- | ---: |
| Address Setup Time | 2 |
| Address Hold Time | 0 |
| Data Setup Time | 25 |
| Bus Turn Around Duration | 0 |
| CLK Division | 默认/无效 |
| Data Latency | 默认/无效 |
| Access Mode | A |

其目的主要是覆盖 ILI9341 ID 类读取的较慢读周期。

#### Write Timing（Extended Mode）

| 参数 | 初始值 |
| --- | ---: |
| Address Setup Time | 2 |
| Address Hold Time | 0 |
| Data Setup Time | 8 |
| Bus Turn Around Duration | 0 |
| Access Mode | A |

168 MHz 下一个 HCLK 约 5.95 ns。

按异步访问近似：

`((ADDSET + 1) + (DATAST + 1)) * tHCLK`

写周期约：

`(3 + 9) * 5.95 ns = 71.4 ns`

高于 ILI9341 8080 写周期最小约 66 ns，适合作为第一版保守值。

注意：

- 上述 Read Timing 面向寄存器/ID 读取。
- 如果后续需要 GRAM/Frame Memory 读回，ILI9341 的读周期要求明显更长，应重新核算，不能继续沿用当前 Read DataSetup。
- LCD 性能优化必须在显示正确后再做，优先使用逻辑分析仪/示波器验证 WR/RD 周期。

### 11.4 LCD Backlight

`PA15 -> GPIO_Output`

| 参数 | 设置 |
| --- | --- |
| Label | `LCD_BL` |
| Initial Level | Low |
| Mode | Output Push Pull |
| Pull | No Pull |
| Speed | Low |

硬件为高有效：

- High：背光亮
- Low：背光灭

建议初始化阶段保持 Low，在 LCD 初始化完成后由 Display 驱动打开背光，避免上电白屏。

### 11.5 LCD Reset

LCD RESET 接 MCU `NRST` 网络。

CubeMX 不分配独立 LCD_RESET GPIO。

软件不能单独硬复位 LCD；MCU Reset 会同时复位屏幕。

---

## 12. XPT2046：GPIO 模拟 SPI

不启用额外 SPI 外设。

| 信号 | 引脚 | CubeMX 模式 | 初始状态 |
| --- | --- | --- | --- |
| T_SCK | PE0 | GPIO Output Push-Pull | Low |
| T_MOSI | PE2 | GPIO Output Push-Pull | Low |
| T_MISO | PE3 | GPIO Input | - |
| T_PEN | PE4 | GPIO Input | - |
| T_CS | PD13 | GPIO Output Push-Pull | High |

建议 GPIO：

- Pull：No Pull
- Speed：Low

其中 T_PEN 已有硬件上拉依据，因此不需要再启用 MCU 内部 Pull-up。

当前基线：

- 不启用 EXTI；
- 不启用 DMA；
- Display/Touch 驱动轮询 T_PEN；
- 触摸校准、旋转方向、滤波参数不属于 CubeMX 配置。

---

## 13. FreeRTOS

CubeMX：

`Middleware -> FREERTOS`

配置：

| 项目 | 设置 |
| --- | --- |
| Interface | CMSIS-V2 |
| Preemption | Enable |
| Tick | 1 ms / 1000 Hz |
| HAL Timebase | TIM2 |
| defaultTask | 保留为最小非业务等待任务 |

S01 初始阶段不在 CubeMX 一次性创建完整业务 Task。

后续按架构逐步增加：

```text
appSystemTask
Display Task
Acquisition Task
Modbus Task
OTA Task
...
```

任务栈大小、Queue、Notification、Mutex 在具体阶段按实际资源测量调整，不在 PREPARATION 阶段提前拍死。

### IRQ 与 FreeRTOS

UART/DMA ISR 后续可能使用 Task Notification / Queue FromISR，因此建议相关中断统一从：

`Preemption Priority = 5`

开始。

不要将调用 FreeRTOS `FromISR` API 的中断设置成高于 `configMAX_SYSCALL_INTERRUPT_PRIORITY` 所允许的优先级。

---

## 14. NVIC 汇总

建议初始配置：

| IRQ | Enable | Preemption Priority | Sub Priority |
| --- | --- | ---: | ---: |
| USART1 | Yes | 5 | 0 |
| DMA2 Stream2 | Yes | 5 | 0 |
| DMA2 Stream7 | Yes | 5 | 0 |
| USART2 | Yes | 5 | 0 |
| DMA1 Stream5 | Yes | 5 | 0 |
| DMA1 Stream6 | Yes | 5 | 0 |
| USART3 | Yes | 5 | 0 |
| DMA1 Stream1 | Yes | 5 | 0 |
| DMA1 Stream3 | Yes | 5 | 0 |
| TIM2 HAL Tick | Yes | 15 | 0 |
| PendSV | Yes | 15 | 0 |
| SysTick | Yes | 15 | 0 |

Priority Group：

`NVIC_PRIORITYGROUP_4`

当前不启用 XPT2046 EXTI。

---

## 15. 当前不用的板载资源

以下板级连接存在，但不属于当前核心 App 基线，CubeMX 暂不启用：

| 资源 | 引脚 |
| --- | --- |
| KEY1/KEY2/KEY3 | PA0 / PA1 / PA4 |
| Blue LED | PB2 |
| Green LED | PC5 |
| CAN1 | PB8 / PB9 |
| USB FS | PA11 / PA12 |
| SDIO | PC8/PC9/PC10/PC11/PC12/PD2 |
| ESP8266 | 当前项目移除 |

不为了“以后可能使用”提前开启外设。

---

## 16. Pin 冲突检查

当前规划中的主要资源不存在直接引脚冲突：

```text
PA2/PA3   -> USART2 / HC-05
PA5/6/7   -> SPI1 / W25Q128
PA9/PA10  -> USART1 / OTA
PA13/14   -> SWD
PA15      -> LCD_BL

PB6/PB7   -> Software I2C
PB10/11   -> USART3 / RS485

PC13      -> W25Q128_CS

PD/PE     -> FSMC LCD + XPT2046 GPIO
```

特别注意：

- PA15 必须从 JTAG 功能释放，因此 SYS 选择 Serial Wire。
- PE0 虽具有 FSMC 复用能力，但当前 LCD 不需要 NBL0，PE0 保留给 XPT2046 软件 SCK。
- PD13 虽在开发板通用引脚表中可能标为空闲，但配套触摸屏接口已将其用于 XPT2046 CS；插屏后不得再分配给其它功能。
- PB6/PB7 虽可作为 I2C1 AF，但本项目明确使用 GPIO 软件 I²C。

---

## 17. CubeMX 建议配置顺序

建议不要一次乱开所有外设，按以下顺序配置并每一步检查 Pinout 冲突：

```text
1. MCU / SYS / Serial Wire
2. RCC + Clock Tree（HSE 频率确认后冻结）
3. PB6/PB7 Software I2C GPIO
4. SPI1 + W25Q128_CS
5. USART1 + DMA + NVIC
6. USART2 + DMA + NVIC
7. USART3 + DMA + NVIC
8. FSMC 16-bit LCD
9. LCD_BL + XPT2046 GPIO
10. FreeRTOS CMSIS-V2
11. SYS Timebase -> TIM2
12. 全局 DMA / IRQ 冲突复核
13. Generate Code
14. Build
```

Generate Code 前至少确认：

- 无红色 Pin Conflict；
- 三路 UART DMA Stream 唯一；
- USART1/2/3 global interrupt 均已启用；
- PA15 没有被 JTAG 占用；
- FSMC D0-D15 / NE1 / NWE / NOE / A16 完整；
- PC13 默认高，避免上电误选 W25Q128；
- PD13 默认高，避免上电误选 XPT2046；
- LCD_BL 默认低；
- PB6/PB7 默认高；
- Clock Tree 没有超频或 48 MHz 域错误。

---

## 18. 尚未关闭的配置项

当前仍应保留为 `TO_VERIFY`：

1. v5.5 开发板实际 HSE 晶振频率，决定 PLLM 最终值。
2. PB6/PB7 板载 I²C 上拉阻值，以及外接 DHT20 模块上拉并联后的等效值。
3. HC-05 当前真实 UART 波特率；如有需要，再通过 AT 模式统一。
4. Modbus RTU 最终 baud/parity/stop bits。
5. v5.5 RS485 电路是否确认采用自动方向切换；确认前不建立 DE/RE GPIO 契约。
6. ILI9341 FSMC 初始时序需要板级显示/读 ID 验证后才能成为最终性能参数。
7. UART RX Circular DMA + IDLE 多实例驱动完成后，需要验证回调位置计算、环形缓冲溢出和长期连续接收。

---

## 19. 依据

项目内：

- `02_Hardware/Pinout/STM32F407VET6_App_外设与引脚配置.md`
- `02_Hardware/Hardware_Software_Interface/2.8寸显示屏/2026-09-26_ILI9341_XPT2046_显示模块硬件事实分析.md`
- `02_Hardware/Hardware_Software_Interface/开发板/STM32F407VE_开发板_原理图_v5.5.pdf`
- 用户提供：`8-2、引脚分配表_按GPIO分类.xlsx`
- Project Owner 最新资源分配：USART1=OTA、USART2=HC-05、USART3=RS485/Modbus；DHT20 外接 PB6/PB7。

官方参考：

- STM32F405/407 Datasheet DS8626  
  https://www.st.com/resource/en/datasheet/stm32f407ve.pdf
- STM32F4 Reference Manual RM0090  
  https://www.st.com/resource/en/reference_manual/dm00031020-stm32f405-415-stm32f407-417-stm32f427-437-and-stm32f429-439-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf
- ST AN2790：TFT LCD interfacing with FSMC  
  https://www.st.com/resource/en/application_note/an2790-tft-lcd-interfacing-with-the-highdensity-stm32f10xxx-fsmc-stmicroelectronics.pdf

说明：

- FSMC 时序计算方法可参考 AN2790；具体 ILI9341 AC Timing 仍以项目采用的实际 ILI9341 数据手册为准。
- DMA Stream/Channel 以 RM0090 的 DMA1/DMA2 request mapping 为准。
