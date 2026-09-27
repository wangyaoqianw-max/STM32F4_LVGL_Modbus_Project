# S02 Verification

## Metadata

- Stage: `S02 板级基础能力 / Platform Bring-up`
- Status: `PASS`
- Design Baseline: `0ffe747eeb0a1e7e2c6a088e2b402bbc57965f44`
- Reuse Baseline: `wangyaoqianw-max/Embedded_Engineering_Library@8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0`
- Implementation Commit: `87762406c602cbe4b896203c833e640b1371ff0c`
- Date: `2026-09-27`
- Branch: `main`

## Environment

- Software target: `STM32F407VET6`；Keil Target: `STM32F407_APP`
- Keil MDK: `5.38.0.0`
- Compiler: `ARM Compiler V5.06 Update 7 (build 960)`
- J-Link: `V7.92`；烧录选择器识别为 `STM32F407VE`，SWD `1 MHz`
- UART host: `COM8`, `115200 8-N-1`；根据当前板级原理图连接至 USART1 PA9/PA10
- 实物开发板型号/版本未通过丝印或独立硬件记录确认

## Reused assets and F407 binding

Library 冻结 Commit 的 22 个源码文件及 MIT `LICENSE` 已逐文件与目标工程做字节比较，结果 `PASS`。迁入资产包括：

- Platform MCU：GPIO、Software I2C、SPI、UART、IRQ、Reset。
- STM32F4 Impl：Delay、GPIO、SPI、UART、IRQ、Reset。
- 不迁入 Watchdog、完整设备 Driver、UART Service、RingBuffer，也未创建 `platform_dma` 或 `platform_fmc`。
- `platform_types.h` 保持冻结 Blob `a2d23e8575f31494e55548bde62c15e7407055b9`，未修改。

项目级 Board Binding 只映射当前工程资源：PB6/PB7 Software I2C、PC5 状态 LED、CubeMX `hspi1` 和 `huart1/2/3`。UART 参数从对应 CubeMX HAL Handle 的 `Init` 配置读取。Binding 只构造 Platform 对象，不调用 HAL 初始化，也不建立系统级初始化链。Keil 工程引用检查与 XML 解析为 `PASS`。

F407 核对结果：`STM32F407VETx` 为 Keil 器件；PB6/PB7 宏映射与当前 `main.h` 一致；GPIO B/C 时钟由 `MX_GPIO_Init()` 开启；USART1/2/3 当前均为 115200、8-N-1、无硬件流控。迁入资产和 Board Binding 中无 F411 专用符号。Platform 公共头没有 HAL Handle 或 GPIO Port/Pin 依赖；APP / Service 静态搜索没有直接访问 HAL Handle、GPIO Port/Pin 或 DMA Handle 的命中。

当前 CubeMX DMA/IRQ 静态核对：USART1 RX `DMA2_Stream2` Circular、TX `DMA2_Stream7` Normal；USART2 RX `DMA1_Stream5` Circular、TX `DMA1_Stream6` Normal；USART3 RX `DMA1_Stream1` Circular、TX `DMA1_Stream3` Normal。DMA 和 USART IRQ 均为 preemption priority 5 / subpriority 0，优先级分组为 `NVIC_PRIORITYGROUP_4`，与 `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY=5` 相符。UART HAL Register Callback 关闭，迁入 Impl 使用的 HAL weak callback 路由与当前工程一致；未发现项目回调冲突。未运行 DMA Runtime。

冻结 IRQ API 还保留 `PLATFORM_MCU_IRQ_KEY_EXTI0` 并映射至 MCU `EXTI0_IRQn`。当前没有将该 ID 绑定到具体板载按键，也没有调用它；这不构成按键接线已确认的证据，未来若启用需先核对引脚复用。

## Results

| 项目 | 结果 | 证据 |
| --- | --- | --- |
| 冻结输入与 Library 资产核对 | `PASS` | 基线 Commit 已记录；22 个源码文件和 LICENSE 与冻结 Commit 逐文件一致 |
| Keil 工程接线 / 架构边界 | `PASS` | 目标文件均存在、工程 XML 可解析；APP / Service 未发现直接 HAL 资源访问 |
| 正式固件 Clean Rebuild | `PASS` | `0 Error / 0 Warning`；Flash `37,960 bytes`，RAM `23,952 bytes`；最终 Clean Rebuild 见下方日志 |
| GPIO Smoke | `PASS` | Platform API 将 PC5 配为输出并写入高、低；随后经 Platform GPIO 读取物理输入电平，两个读回值均符合预期 |
| Software I2C ACK Probe | `PASS` | PB6/PB7 无内部上下拉采样为空闲高后，Platform I2C 对 7-bit 地址 `0x50` 探测收到 ACK；未执行 EEPROM 写入 |
| USART1 Blocking TX/RX | `PASS` | COM8 发送 `S02_PING`；接收 `S02_PINGS02_UART_ECHO_PASS` 和各 Smoke 状态行，共 `80 bytes` |
| SPI1 Platform 生命周期 | `PASS` | 临时 Smoke 经 `hspi1` Binding 执行 Platform init/start/stop/deinit；未进行 SPI 设备事务 |
| 正式固件恢复 | `PASS` | 移除临时测试后重新 Clean Rebuild；J-Link Flash 校验 `verified=true` |

最终 Clean Rebuild 日志：`06_Output/S02/BuildLogs/final-closure/STM32F407_APP-STM32F407_APP-rebuild.log`（`0 Error / 0 Warning`）。测试入口移除后的正式固件构建日志：`06_Output/S02/BuildLogs/final/STM32F407_APP-STM32F407_APP-rebuild.log`。捕获 Smoke 镜像的构建日志：`06_Output/S02/BuildLogs/smoke-capture/STM32F407_APP-STM32F407_APP-rebuild.log`。构建产物与工具运行记录保留在被 Git 忽略的 `06_Output/S02/`。

Smoke 串口回读的状态行为：

```text
S02_PINGS02_UART_ECHO_PASS
S02_GPIO_PASS
S02_I2C_ACK_PASS
S02_SPI_BIND_PASS
```

GPIO 读回路径读取 MCU GPIO 输入数据寄存器；本次没有外接逻辑分析仪或独立电压表记录。I2C 空闲高电平和 ACK 来自真实开发板运行结果；未记录总线波形。

测试期间临时加入 Keil 的 Smoke 源文件、Include Path、工程组和 `main.c` 调用均已移除。正式固件中没有测试逻辑残留。

## Deferred / Pending / Not Run

### `DEFERRED`

- SPI 真实器件通信、W25Q128 JEDEC ID / Read：S03。
- UART DMA + IDLE + RingBuffer Runtime：S04；USART2/3 未额外做阻塞板测，按计划只验证 USART1。
- FMC / LCD 实机时序与显示：S06；本阶段仅确认 CubeMX/HAL `MX_FSMC_Init()` 配置保留。
- Reset 独立人工板测：设计未安排，本阶段不执行。

### `PENDING / TO_VERIFY`

- PB6/PB7 外部上拉电阻阻值仍未测量。当前实物总线空闲高且 `0x50` 收到 ACK，不能据此推断外部电阻值。
- 实物开发板型号/版本尚未通过板卡丝印或独立记录确认；J-Link 仅识别目标为 `STM32F407VE`。
- 冻结 IRQ API 中的 `KEY_EXTI0` 语义没有与本板具体按键连接核对；当前无调用，后续启用前需确认物理引脚与复用。
- HSE 实际晶振频率仍未确认。当前 `SystemClock_Config()` 使用 HSI；`.ioc` 的 `HSE_VALUE=25 MHz` 不作为实物晶振证据。
- Pinout 文档记载 `PLL48CLK=48 MHz`，当前 `.ioc` 为 HSI + PLL、SYSCLK `168 MHz`、`PLLQCLK=84 MHz`。时钟配置未在 S02 修改，文档/设计意图待后续核对。
- USART2/3 的 Platform 阻塞收发 Runtime 未运行；若后续选择它们作为 S04 消费者，应在该阶段核实其 DMA/IRQ 资源使用。

### `INFO`

- Library `original/LICENSE` 声明 MIT，但迁入源文件保留的头部包含 `All Rights Reserved`。本次按冻结资产要求保留源文件原文并携带 LICENSE，没有自行改写授权文字；后续分发前应核实来源仓库的授权表述。

### `NOT_RUN`

- 逻辑分析仪对 GPIO 波形或 I2C START / 地址 / ACK / STOP 的外部采集。本阶段已通过 GPIO 输入读回和真实 I2C ACK 验证最低链路，未重复进行额外仪器测试。

## Final verification state

- Code / build verification: `PASS`。
- 本计划要求的 GPIO、Software I2C ACK、USART1 Blocking TX/RX：真实板级 `PASS`。
- SPI 仅 Platform 生命周期与 Binding `PASS`；设备通信保持 `DEFERRED`。
- 未确认硬件事实保持 `PENDING / TO_VERIFY`，不影响本阶段已定义的 Binding 验收。
- Independent Review：`PASS`；Review 文件见 `00_Project/03_Stages/S02_板级基础能力与Platform_Bring-up/review.md`。
- S02 已关闭；下一工作项为 S03 Design / Implementation Plan。
