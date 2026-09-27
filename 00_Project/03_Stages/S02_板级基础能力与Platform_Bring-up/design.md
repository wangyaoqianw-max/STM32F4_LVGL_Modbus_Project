# S02 Design

## Metadata

- Stage: `S02 板级基础能力 / Platform Bring-up`
- Status: `DESIGN_APPROVED`
- Owner: `Project Owner`
- Date: 2026-09-27
- Design Baseline Commit: `0ffe747eeb0a1e7e2c6a088e2b402bbc57965f44`
- Reuse Baseline: `wangyaoqianw-max/Embedded_Engineering_Library@8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0`

## Goal

在 S01 已形成的 STM32F407VET6 Application 母工程上，迁入并适配可复用的 MCU Platform / STM32F4 Impl 基础资产，完成与当前 CubeMX/HAL 底层资源的绑定，为后续 Driver、Service 和设备功能开发提供稳定底座。

本阶段的核心目标是：

```text
CubeMX / HAL 已生成资源
        ↓
STM32F407 Impl Backend
        ↓
Board / HAL Resource Binding
        ↓
Platform API 可用
        ↓
后续阶段再接 Driver / Service
```

S02 不追求完整设备初始化链，也不提前实现后续设备驱动和业务服务。

## Design principles

1. 复用已有成熟基础资产，不因为更换 F411 → F407 而重写通用 Platform API。
2. 迁入范围可以大于本阶段板测范围；“进入工程”不等于“本阶段必须完成完整功能验证”。
3. 当前只完成能够确定的底层资源绑定，不伪造尚不存在的 Driver / Service 初始化关系。
4. HAL Handle、GPIO Port/Pin、IRQ 等具体硬件事实只存在于 Impl / Board Binding 一侧，不向 Service 泄漏。
5. 不为了架构完整性创建当前没有真实消费者的抽象层。
6. 未验证硬件事实继续保持 `TO_VERIFY`。

## Inputs and constraints

### Current Application baseline

目标工程：

`03_Firmware/Application/STM32F407_APP/`

当前已经具备：

- STM32F407VET6 CubeMX/HAL 工程；
- FreeRTOS CMSIS-V2；
- HSI + PLL 168 MHz 系统主时钟；
- SPI1；
- USART1 / USART2 / USART3；
- UART DMA 与 USART IRQ 的 CubeMX 资源配置；
- FSMC 16-bit LCD 资源配置；
- PB6/PB7 Software I2C GPIO；
- XPT2046 GPIO；
- S01 五层基础框架；
- RTT + EasyLogger + CmBacktrace 诊断基础。

### Reuse source

优先复用：

`wangyaoqianw-max/Embedded_Engineering_Library`

冻结参考 Commit：

`8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0`

当前允许迁入的 MCU 基础资产：

- GPIO；
- Delay；
- Software I2C；
- SPI；
- UART；
- IRQ；
- Reset；
- 对应 STM32F4 Impl Backend。

Watchdog 暂不迁入 S02。

`platform_types.h` 继续保持 S01 冻结约束，不因本阶段迁移而重构或风格修改。

## Scope

### In scope

- 审查并迁入上述 Platform MCU / STM32F4 Impl 基础资产。
- 清理来源工程中的 F411 板级假设，适配当前 F407 工程。
- 完成当前已存在 HAL Handle、GPIO 和 IRQ 等资源的底层 Binding。
- 对 GPIO、Software I2C、UART 执行与本阶段目标相称的最小板级 Smoke Test。
- 对 SPI 完成 Platform/Impl 接入、`hspi1` 绑定和构建/初始化路径检查，实际设备通信留 S03。
- 对 IRQ/DMA 做资源映射、优先级和回调链静态/构建检查；正式 DMA Runtime 留 S04。
- 保留 FMC 当前 CubeMX/HAL 配置并检查资源一致性，不创建通用 `platform_fmc`。
- 更新 S02 验证、交接、Review 和项目状态。

### Out of scope

- Watchdog / Health Policy；
- W25Q128 正式 Driver、读写/擦除和 Font Access；
- UART DMA + IDLE + RingBuffer Runtime；
- UART Service；
- DHT20 Driver / Acquisition Service；
- AT24C02 完整 Driver / Persistence；
- ILI9341、XPT2046、LVGL；
- RS485 / Modbus；
- OTA / YMODEM；
- HC-05 业务能力；
- Bootloader；
- 新增通用 `platform_dma`；
- 新增通用 `platform_fmc`。

## Module boundary

### Platform / Impl relationship

```text
APP / Service / Driver
        ↓
Platform API
        ↓
STM32F4 Impl
        ↓
Board / HAL Binding
        ↓
HAL Handle / GPIO / IRQ
```

S02 的施工终点主要位于 `Platform → Impl → HAL`。

Driver 和 Service 只在后续阶段按真实设备需求加入。

### Binding ownership

具体硬件资源必须留在 Impl / Board 侧，例如：

```text
SPI1   → hspi1
USART1 → huart1
USART2 → huart2
USART3 → huart3

Software I2C:
SCL → PB6
SDA → PB7
```

绑定实现优先使用 Library 已有接口。若现有接口不能直接承接当前 F407 CubeMX 资源，可在 `04_Impl/impl_board` 增加薄型 Binding Adapter，但不得把设备业务语义塞入通用 Platform。

S02 不建立全局 `board_init_all()` 一类提前固化所有未来设备的初始化总入口。

## Per-module decisions

| Module | S02 action | S02 physical verification | Deferred |
| --- | --- | --- | --- |
| GPIO | 迁入并绑定 | 最小可观测 GPIO Smoke | 设备业务 GPIO 策略按后续模块 |
| Delay | 迁入 | 随 Software I2C 间接验证 | 更高精度/RTOS Delay 按真实需求 |
| Software I2C | 迁入并绑定 PB6/PB7 | AT24C02 地址 ACK + 逻辑分析仪优先 | DHT20 S05；AT24C02 Persistence S08 |
| SPI | 迁入并绑定 `hspi1` | 本阶段不强制真实设备事务 | W25Q128 JEDEC/读写 S03 |
| UART | 迁入并绑定 `huart1/2/3` | 选择最方便的一路做 Blocking TX/RX Smoke | DMA+IDLE+RingBuffer S04 |
| IRQ | 迁入/适配 | 随现有运行链间接验证，重点静态检查 | 复杂 ISR→Task S04 |
| Reset | 迁入 | 不专门做板测 | OTA/Bootloader 使用时再验 |
| DMA | 不新增通用 Platform 抽象 | 仅检查 CubeMX mapping / priority | UART DMA Runtime S04 |
| FMC | 保留 CubeMX/HAL | 不做 LCD/FMC 实机验收 | ILI9341/FMC timing S06 |
| Watchdog | 不迁入 | 不测试 | S10/S12 可靠性阶段 |

## Minimal hardware verification

### GPIO Smoke

选择 Pinout/原理图中已确认且可直接观察的 GPIO 进行最小输出验证。

验证目标仅为：

```text
Platform GPIO
    ↓
STM32F4 GPIO Impl
    ↓
HAL
    ↓
Physical Pin
```

不得在未核对原理图时自行假定 LED 有效电平或外部负载关系。

### Software I2C bus probe

使用 PB6/PB7 当前 Software I2C 配置，对板载 AT24C02 做最小地址 ACK 探测。

约束：

- 不引入完整 AT24C02 Driver；
- 不写 EEPROM 数据；
- 优先使用逻辑分析仪确认 START / Address / ACK / STOP；
- PB6/PB7 外部上拉仍是 `TO_VERIFY`；
- 若总线不能释放为高电平，先验证物理上拉事实，不通过随意打开内部上拉掩盖硬件问题。

该测试用于同时验证 GPIO Open-Drain、Delay、Software I2C 时序和实际 I2C 线路。

### UART Smoke

从当前可实际连接的 USART1/2/3 中选择最方便的一路，完成最小 Blocking TX/RX。

本阶段不要求：

- 三路 UART 全部做重复板测；
- DMA Circular RX；
- IDLE；
- RingBuffer；
- Task Notification；
- UART Service。

## SPI / DMA / FMC deferred verification rationale

SPI1 的第一个明确真实消费者是 S03 W25Q128，因此 S02 再做独立 SPI Loopback 会造成重复测试。S03 使用 JEDEC ID / Read 作为更有意义的 SPI 物理链路证据。

DMA 当前主要由 CubeMX/HAL 与 UART 使用，尚无独立上层消费者，因此不创建 `platform_dma`。S04 用真实 UART DMA Runtime 验证。

FMC 当前明确消费者是 ILI9341 LCD。S02 保留资源配置即可；在没有稳定跨设备抽象需求前创建 `platform_fmc` 只会包装 HAL。真实总线时序和显示验证进入 S06。

## Confirmed facts / Design choices / To Verify

### Confirmed facts

- S01 Application 母工程和 Diagnostics 已验证并关闭。
- 当前系统使用 HSI + PLL 168 MHz。
- SPI1、USART1/2/3、UART DMA、USART IRQ、FSMC、Software I2C GPIO 已进入 CubeMX 基线。
- Embedded Engineering Library 当前冻结版本具备可复用的 STM32F4 GPIO/SPI/UART/Software I2C/IRQ/Reset/Delay 基础资产。
- UART/SPI 资产采用 Handle 注入思路，不要求 Service 直接访问全局 HAL Handle。

### Design choices

- MCU Platform 基础资产整体迁入，Watchdog 暂缓。
- S02 只冻结底层 Binding，不冻结未来完整 Driver / Service 初始化顺序。
- SPI 真正物理验证进入 S03。
- UART DMA Runtime 进入 S04。
- 不新增 `platform_dma` / `platform_fmc`。
- Reset 允许进入工程，但不安排独立人工板测。

### TO_VERIFY

继续保留：

- HSE 实际晶振频率；
- 实物板卡身份的独立确认；
- PB6/PB7 实际外部上拉；
- HC-05 实际 UART 参数；
- Modbus 正式 UART 参数；
- RS485 收发方向控制；
- LCD/FSMC 实机时序；
- Pinout 文档 PLL48 描述与当前实际未使用 PLL48 域的一致性；
- J-Link 历史间歇连接失败原因，仅在复现时调查。

## Failure and recovery rules

- Library 资产若因 F407 与来源工程差异无法直接编译，先定位差异属于 MCU、HAL、CubeMX 资源还是来源工程板级假设，不直接修改 Platform API 掩盖问题。
- 发现接口必须暴露 HAL Handle 才能工作时，先进入设计复审。
- Software I2C ACK 失败时先检查线路、电平、上拉和时序，不直接判断 Driver 设计失败。
- UART 板测失败先拆分 Host/接线/波特率/HAL/Platform 层，不提前引入 DMA/RingBuffer 解决。
- SPI、FMC、DMA 未做本阶段真实设备测试不能写成 PASS，只记录 S02 实际完成的构建/静态验证。
- 若施工发现真实需求需要 Watchdog、`platform_dma` 或 `platform_fmc`，先回到设计审查，不顺手扩展。

## Acceptance criteria

- GPIO、Delay、Software I2C、SPI、UART、IRQ、Reset 的选定 Library 资产已迁入并通过当前 F407 工程适配。
- Watchdog 未因“资产完整”而提前迁入。
- HAL Handle / GPIO / IRQ 绑定位置和所有权明确，上层不直接依赖具体 STM32 Handle。
- Clean Rebuild `PASS`；记录实际 Error/Warning。
- GPIO 最小 Smoke `PASS`。
- Software I2C 对 AT24C02 地址 ACK 的真实板级验证有证据；若受物理上拉等未确认条件阻塞，必须记为 `BLOCKED/PENDING`，不得伪造 PASS。
- 至少一路 UART Blocking TX/RX Smoke 有真实结果。
- SPI 在 S02 至少完成构建与 Binding 验证，但不把未执行的 W25Q128 通信写成 PASS。
- 不新增通用 `platform_dma`、`platform_fmc`。
- 不提前引入 W25Q128、DHT20、AT24C02 完整 Driver、UART Service、RingBuffer、LVGL、Modbus、OTA。
- `platform_types.h` 保持冻结内容。
- 未确认硬件事实继续保持 `TO_VERIFY`。
- 阶段 Verification / Handoff / Review 与项目状态文档一致。

## Required reading

1. `AGENTS.md`
2. `00_Project/WORKFLOW.md`
3. `PROJECT_CONTEXT.md`
4. `00_Project/05_Status/current_status.md`
5. `00_Project/03_Stages/S01_Application工程初始化与诊断基础/handoff.md`
6. `00_Project/03_Stages/S01_Application工程初始化与诊断基础/review.md`
7. `04_Test/Reports/Stages/S01/verification.md`
8. `02_Hardware/Pinout/STM32F407VET6_CubeMX_外设与引脚配置基线.md`
9. `03_Firmware/00_Doc/嵌入式C代码规范.md`
10. Embedded Engineering Library `8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0`
