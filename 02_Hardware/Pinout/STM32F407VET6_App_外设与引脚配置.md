# STM32F407VET6 App 外设与引脚配置梳理

- 日期：2026-09-26
- 工作项：`PREPARATION`
- 用途：创建 Application 工程前，按功能模块汇总当前可用的外设与引脚依据。
- 状态：静态资料整理；Application 工程尚未创建，未进行板级验证。

> 本表是 App 工程准备入口，便于核对模块资源。开发板原理图及对应的硬件事实分析仍是板级连接依据；商家例程只提供实现参考，不能单独冻结本工程配置。

## 状态说明

- `CONFIRMED`：证据直接确认其声明范围；若来源是商家例程，只确认例程配置，不延伸为本项目板级事实。
- `INFERRED`：可由已确认事实推导出候选，但仍需原理图、BOM 或实测复核。
- `UNRESOLVED`：本项目尚无足够证据确定引脚、器件参数或外设实例。

## 模块资源总览

| 功能模块 | 外设/方式 | 当前引脚或候选配置 | 状态 | App 创建时的边界 |
|---|---|---|---|---|
| 显示（ILI9341/LVGL） | 16 位 8080 并口，经 STM32F407 FSMC | 见下方 LCD 引脚表；接口网络在图纸中标作 `FMC` | `CONFIRMED` | 芯片外设名称使用 `FSMC`；总线占用较多 PD/PE 引脚，时钟及读写时序仍需按本工程时钟树核算。 |
| 触摸（XPT2046） | GPIO 模拟 SPI；PENIRQ 轮询 | `PE0/PE2/PE3/PE4`、`PD13` | `CONFIRMED` | 商家实现未使用硬件 SPI、DMA 或 EXTI；触摸坐标方向和校准值需在目标屏上确定。 |
| 温湿度与启动元数据 | `PB6/PB7` 软件 I²C，共享总线 | `PB6=SCL`，`PB7=SDA`；挂接 DHT20、AT24C02 | `CONFIRMED` | 软件 I²C 是共享资源；总线事务需统一仲裁。器件地址、上拉、电气条件和速率待复核。 |
| 外部 Flash（字库/OTA） | W25Q128，SPI | 商家例程：`SPI1`，`PA5=SCK`、`PA6=MISO`、`PA7=MOSI`、`PC13=CS` | 例程配置 `CONFIRMED`；本项目物理引脚 `UNRESOLVED` | 作为候选核对开发板原理图；JEDEC 料号、SPI 参数和项目分区均未冻结。 |
| Modbus RTU / RS485 | 半双工 UART | 商家例程：`USART3`，`PB10=TX`、`PB11=RX`；本项目 UART 未分配 | 例程配置 `CONFIRMED`；本项目分配 `UNRESOLVED` | App 设计要求 Modbus 独占 UART；需确认收发器连接、`DE/RE` 控制脚/极性，并检查 DMA Stream/Channel 与 IRQ 冲突。 |
| 有线 OTA | 独立 UART + YMODEM | 未分配 | `UNRESOLVED` | 按设计决策与 Modbus、HC-05 使用不同 UART；Bootloader/App 的串口访问边界需按阶段设计落实。 |
| HC-05 蓝牙 | 独立 UART（后续规划） | 未分配 | `UNRESOLVED` | 不与 Modbus 或 OTA 共用实时 UART；当前不作为 App 启动必需功能。 |
| 调试/日志 | SWD；调试串口待分配 | 商家 `.ioc` 使用 `PA13=SWDIO`、`PA14=SWCLK`；商家测试串口为 `USART1 PA9/PA10` | 例程配置 `CONFIRMED`；本项目连接/用途 `UNRESOLVED` | 日志 UART 不得混入 Modbus、OTA 或 HC-05 数据通道；SWD 接口以板卡原理图复核。 |

## 已确认的显示与触摸引脚

### ILI9341 16 位并口

| LCD 信号 | STM32F407 引脚 | FSMC 信号/说明 | 状态 |
|---|---|---|---|
| CS | `PD7` | `FSMC_NE1` | `CONFIRMED` |
| WR | `PD5` | `FSMC_NWE` | `CONFIRMED` |
| RD | `PD4` | `FSMC_NOE` | `CONFIRMED` |
| RS/DC | `PD11` | `FSMC_A16` | `CONFIRMED` |
| D0–D3 | `PD14, PD15, PD0, PD1` | `FSMC_D0–D3` | `CONFIRMED` |
| D4–D12 | `PE7–PE15` | `FSMC_D4–D12` | `CONFIRMED` |
| D13–D15 | `PD8–PD10` | `FSMC_D13–D15` | `CONFIRMED` |
| RESET | `NRST` | 与 MCU 复位网络共用，不能独立 GPIO 复位屏幕 | `CONFIRMED` |
| 背光控制 | `PA15` | `LCD_BL`，高有效开背光 | `CONFIRMED` |

本项目详细依据见[显示模块硬件事实分析](../Hardware_Software_Interface/2.8寸显示屏/2026-09-26_ILI9341_XPT2046_显示模块硬件事实分析.md)及[开发板原理图](../Hardware_Software_Interface/开发板/STM32F407VE_开发板_原理图_v5.5.pdf)。F407 芯片手册和商家代码称该控制器为 `FSMC`；原理图中的 `FMC_Dx` 是网络名，不表示另有 FMC 外设。

### XPT2046 触摸

| 触摸信号 | STM32F407 引脚 | 商家例程用法 | 状态 |
|---|---|---|---|
| `T_SCK` | `PE0` | GPIO 模拟时钟 | `CONFIRMED` |
| `T_MOSI` | `PE2` | GPIO 输出 | `CONFIRMED` |
| `T_MISO` | `PE3` | GPIO 输入 | `CONFIRMED` |
| `T_PEN/PENIRQ` | `PE4` | 低有效输入，轮询 | `CONFIRMED` |
| `T_CS` | `PD13` | 低有效 GPIO 片选 | `CONFIRMED` |
| `BUSY` | 未接 MCU | 不使用 | `CONFIRMED` |

触摸 GPIO 与 LCD 的 FSMC 数据/控制组分开。屏幕旋转、RGB/BGR 顺序、LVGL 色深和触摸坐标校准值仍属于 App/板级验证项。

## 共享软件 I²C

| 设备 | 总线信号 | 引脚 | 本项目关系 | 状态 |
|---|---|---|---|---|
| DHT20 | SCL/SDA | `PB6/PB7` | 与 AT24C02 共用软件 I²C | `CONFIRMED`（设计基线） |
| AT24C02 | SCL/SDA | `PB6/PB7` | 保存少量启动/OTA 元数据；与 DHT20 共用总线 | `CONFIRMED`（设计基线） |

商家例程的 `BSP/I2C/bsp_I2CSoft.h:32-36` 也将软件 I²C 定义为 PB6/PB7，并包含 AT24C02 驱动。该例程没有本项目 DHT20 驱动，因此 DHT20 的挂接依据来自本项目设计决策，而非商家例程。

## 商家例程中的候选配置

商家示例根目录：

`E:\嵌入式资料\STM32F407VET6资料\5、标准例程--HAL库版本\99-9  发货前综合测试\示例代码`

| 例程模块 | 例程配置 | 代码位置 | 本项目使用边界 |
|---|---|---|---|
| W25Q128 | `SPI1`；`PA5=SCK`、`PA6=MISO`、`PA7=MOSI`、`PC13=CS` | `BSP/W25Q128/bsp_W25Q128.h:58-76`；`BSP/XPT2046/bsp_XPT2046.c:2` | 作为新板连接候选；不得据此确认料号后缀、接线或 App 分区。例程字库起始地址为 `0x00A00000`，触摸校准数据也靠近该区域；本项目分区未定，不能照搬地址。 |
| UART3 | `PB10/PB11`；`Core/Src/main.c` 以 115200 初始化，并用于 RS485/ESP8266 测试通道 | `BSP/UART/bsp_UART.h:142`；`BSP/UART/bsp_UART.c:811`；`Core/Src/main.c:153, 201, 264-265` | 仅作例程配置参考。例程把通道用于多个测试对象，与本项目 Modbus、OTA、HC-05 分 UART 的设计要求不同。例程 UART3 通过 `USART3_IRQHandler` 的 TXE/RXNE 中断收发，不是本项目计划的 DMA/IDLE 实现；当前检索到的 UART3 代码没有明确的 `DE/RE` GPIO 控制配置。 |
| USART1 | `PA9=TX`、`PA10=RX`；例程 CubeMX 初始化并用于测试/日志 | `STM32F407.ioc:17-22, 72-84`；`Core/Src/main.c:133` | 不视为本项目已分配的调试或 OTA UART。 |
| CAN1、按键、LED | `PB8=CAN1_RX`、`PB9=CAN1_TX`；按键 `PA0/PA1/PA4`；LED `PC5/PB2` | `STM32F407.ioc:25-39, 92-95`；`Core/Inc/main.h:60-72` | 属于商家综合测试例程资源，不属于当前 App 模块基线；是否保留要由本项目需求和原理图决定。 |
| SWD | `PA13=SWDIO`、`PA14=SWCLK` | `STM32F407.ioc:27, 38, 74, 76` | 例程的 MCU 调试配置；仍按本项目开发板原理图确认调试接口。 |
| 时钟 | 例程 `.ioc` 采用 25 MHz HSE，配置计算值为 168 MHz SYSCLK/HCLK | `STM32F407.ioc` 中 `RCC.HSE_VALUE`、`RCC.HCLKFreq_Value` | 只作例程参数记录；本项目时钟树未冻结，FSMC 时序必须按本工程实际时钟重新计算。 |

该 `.ioc` 只声明 CAN1、USART1、RCC、SYS 等 CubeMX 项；软件 I²C、W25Q128 SPI 和屏幕/触摸 BSP 的配置位于手写模块代码中。因此 `.ioc` 不是商家例程的完整引脚清单，更不是本项目 App 的可直接复制配置。

## 创建 App 工程前仍需关闭

1. 对照开发板原理图确认 W25Q128 的物理接线、完整料号和 SPI 资源；确认后再建立 W25Q128 驱动和 Flash 分区契约。
2. 为 Modbus、UART/YMODEM OTA、HC-05 分别确定 UART 实例与引脚；统一核查 DMA Stream/Channel、IRQ、优先级和日志串口占用。
3. 从 RS485 收发器接线确认 `DE/RE` 的控制方式、MCU 引脚和空闲电平；不要从 UART3 的 TX/RX 定义推断方向控制已完成。
4. 复核 PB6/PB7 上拉、电平、设备地址、总线速率和恢复边界；App 侧通过共享总线服务做事务级互斥。
5. 冻结本项目 HCLK/FSMC Bank 与时序、LCD 方向/颜色顺序、XPT2046 校准方式；所有时序按本项目时钟重新核算。
6. 确认 SWD/日志调试接口在开发板上的实际连接，并确保日志不会占用协议数据通道。

## 证据索引

- 本项目设计基线：[STM32F407VET6 工程设计决策](../../00_Project/04_Decisions/STM32F407VET6_工业控制从机综合项目_设计决策.md)，重点见 §3.1、§3.2、§5、§11、§12。
- 显示与触摸板级证据：[ILI9341/XPT2046 显示模块硬件事实分析](../Hardware_Software_Interface/2.8寸显示屏/2026-09-26_ILI9341_XPT2046_显示模块硬件事实分析.md)，重点见 §3–§6。
- 开发板原始资料：[STM32F407VE 开发板原理图 v5.5](../Hardware_Software_Interface/开发板/STM32F407VE_开发板_原理图_v5.5.pdf)。
- 商家例程：上述本机路径；本轮只读检查源码/`.ioc`，未构建或重新进行板级验证。此前记录的同板运行信息不等同于本轮验证，也不替代本项目引脚/资源冻结。


