# 开发路线图

> 版本：V0.1  
> 状态：粗粒度路线，用于确定总体施工方向和主要依赖关系。  
> 本路线**不冻结各阶段的完整边界、详细任务和 DoD**。进入具体阶段前，应结合当时的硬件事实、仓库状态、测试结果和实际需求重新讨论，并允许拆分、合并、前移、后移或调整实现方向。  
> 活动状态仍以 `00_Project/05_Status/current_status.md` 为准。

## 1. 总体路线

```text
S00 工程准备与硬件事实整理
        ↓
S01 Application 工程初始化与诊断基础
        ↓
S02 板级基础能力 / Platform Bring-up
        ↓
S03 W25Q128 与中文字库基础
        ↓
S04 FreeRTOS Runtime / UART DMA 通信基础
        ↓
S05 Sensor Acquisition / Application Data Model
        ↓
S06 LCD + Touch + LVGL HMI
        ↓
S07 RS485 + Modbus RTU Slave
        ↓
S08 Storage / Persistence
        ↓
S09 OTA Application Service
        ↓
S10 Bootloader / Trial / Confirm / Rollback
        ↓
S11 可选扩展能力
        ↓
S12 系统集成 / Reliability / Delivery
```

| 阶段 | 方向 | 当前粗粒度目标 |
| --- | --- | --- |
| S00 | 工程准备与硬件事实整理 | 原理图、Pinout、外设资源、DMA/IRQ、工具链和资料基线 |
| S01 | Application 工程初始化与诊断基础 | CubeMX/Keil APP 母工程、FreeRTOS 基础、RTT + EasyLogger、cmBacktrace、Build/Flash/Log 基础链路 |
| S02 | 板级基础能力 / Platform Bring-up | GPIO、SPI、软件 I2C、UART、FMC、基础 IRQ/DMA 等底层能力验证 |
| S03 | W25Q128 与中文字库基础 | W25Q128 基础驱动、商家布局确认、字库区域保护、字体读取基础 |
| S04 | FreeRTOS Runtime / UART DMA 通信基础 | Task/IPC/资源所有权，以及 UART + DMA + IDLE + RingBuffer |
| S05 | Sensor Acquisition / Application Data Model | DHT20、采集服务、统一样本/状态模型、Application Snapshot |
| S06 | LCD + Touch + LVGL HMI | ILI9341、XPT2046、LVGL、中文字库和 Display Task |
| S07 | RS485 + Modbus RTU Slave | RS485 半双工、Modbus RTU、寄存器映射和 PC 主机联调 |
| S08 | Storage / Persistence | AT24C02 Metadata、W25Q128 正式分区、持久化与访问协调 |
| S09 | OTA Application Service | OTA UART、YMODEM、候选镜像、校验和 Pending 状态 |
| S10 | Bootloader / Trial / Confirm / Rollback | Bootloader 安装、Trial、Confirm、Rollback、Watchdog 和恢复 |
| S11 | 可选扩展能力 | HC-05 等非核心能力；不阻塞主线完成 |
| S12 | 系统集成 / Reliability / Delivery | 长稳、故障注入、资源测量、异常恢复、Release 和交付资料 |

## 2. 当前路线说明

### S00 工程准备与硬件事实整理

当前重点仍是确认硬件事实，不提前用假设补全未知项。包括：

- 原理图和 MCU Pinout；
- FMC/LCD、XPT2046、W25Q128、AT24C02、DHT20、RS485、UART 等连接；
- DMA / IRQ 资源；
- 软件 I2C；
- 调试接口和工具链版本；
- 未确认项继续保留为 `Unknown / To Verify`。

已确认的板级约束：

- 开发板 MCU 引脚基本由芯片直接引出至排针；
- 板上没有针对任意外部用途提供通用输入/输出隔离保护；
- 外接电路时，需要根据实际用途自行考虑限流、电平匹配、隔离、感性负载保护等；
- 后续若加入电机、继电器或其他执行器，必须重新进行硬件保护方案审查。

### S01 Application 工程初始化与诊断基础

先建立一个可以长期施工的 Application 母工程，重点方向包括：

- STM32F407VET6 CubeMX/HAL + Keil 工程；
- 已确认 Pinout 和基础配置进入 CubeMX 基线；
- FreeRTOS 基础配置；
- Keil 构建链路；
- J-Link / RTT；
- EasyLogger；
- cmBacktrace；
- 后续重新 Generate Code 时，对诊断工具和必要工程修改建立可恢复、可验证的机制。

本阶段目标首先是做到：

```text
能稳定 Build
能下载
能看日志
出现 Fault 能定位
```

具体的诊断工具移植恢复脚本、patch 或检查规则，等该阶段正式实施时再根据对照实验结果确定。

### S02 板级基础能力 / Platform Bring-up

开始验证 F407VET6 新开发板的基础硬件通路：

```text
GPIO
SPI
软件 I2C
UART
FMC
基础 IRQ / DMA
```

这一阶段优先确认 MCU 到外设的基础能力成立，不要求一次完成所有上层 Service。

### S03 W25Q128 与中文字库基础

W25Q128 需要较早加入，因为开发板商家已在 Flash 后部预烧录中文字库。

当前商家资料说明：

- W25Q128 总容量为 16 MiB；
- 前约 10 MiB 作为用户可用区域；
- 后约 6 MiB 预烧录中文字库；
- 具体字库起始地址、各字号布局、编码方式和寻址算法仍需通过商家示例与实机读取确认。

本阶段优先：

```text
SPI
 ↓
W25Q128
 ↓
JEDEC / Read
 ↓
商家存储布局验证
 ↓
预烧录字库
 ↓
Font Access
```

在字库布局确认前，优先只读验证，不进行可能破坏预装字库的 Chip Erase 或未知区域擦写。

### S04 FreeRTOS Runtime / UART DMA 通信基础

S01 只建立 FreeRTOS 基础，本阶段再形成正式 Runtime：

- Task ownership；
- Queue / Notification / Mutex；
- ISR → Task；
- UART + DMA + IDLE + RingBuffer。

为后续 Modbus、OTA、HC-05 等通信能力提供统一基础。

### S05 Sensor Acquisition / Application Data Model

核心数据链路：

```text
DHT20
 ↓
Sensor Driver
 ↓
Acquisition Service
 ↓
Application Snapshot
```

重点不是只把 DHT20 读出来，而是建立统一的 Application Data Model，使 LVGL 和 Modbus 均依赖应用快照，而不是直接访问传感器。

### S06 LCD + Touch + LVGL HMI

建立本地 HMI：

```text
FMC → ILI9341 → LVGL ← Application Snapshot
                         ↑
XPT2046 → Calibration / Filter
```

中文字库通过 W25Q128 的字体访问能力接入 LVGL。

Display Task 原则上保持为 LVGL 的唯一 owner。

### S07 RS485 + Modbus RTU Slave

建立对外通信闭环：

```text
PC Master
   ↕
RS485
   ↕
Modbus RTU Slave
   ↓
Register Mapping
   ↓
Application Snapshot
```

到这一阶段完成时，项目应形成第一版核心 MVP：

```text
采集
 ↓
状态管理
 ├──→ LVGL
 └──→ Modbus
```

### S08 Storage / Persistence

在 W25Q128 和 AT24C02 基础能力已验证后，再建立正式存储职责：

- AT24C02：Boot / OTA Metadata；
- W25Q128：Font、OTA Image、后续 Config / Log、Reserved；
- 处理分区、访问所有权、互斥、写入策略和掉电一致性。

初期不为了“完整存储系统”引入文件系统。

### S09 OTA Application Service

Application 侧负责：

```text
PC
 ↓
UART + YMODEM
 ↓
OTA Service
 ↓
W25Q128 Candidate Image
 ↓
校验
 ↓
AT24C02 = Pending
```

Application 不负责最终覆盖自己的运行区。

### S10 Bootloader / Trial / Confirm / Rollback

Bootloader 侧负责：

```text
Reset
 ↓
Bootloader
 ↓
读取 Metadata
 ↓
验证 Candidate
 ↓
安装 Firmware
 ↓
Trial
 ↓
Confirm / Rollback
```

复用已有 Bootloader / OTA 项目的成熟思路，但重新建立 F407VET6 的启动文件、Flash Layout、Linker 和板级实现。

### S11 可选扩展能力

当前主要候选为 HC-05 Bluetooth Classic SPP：

- 诊断；
- 参数通道；
- 透明通信；
- 后续可评估复用 OTA transport。

该阶段不应成为核心主线 blocker。

### S12 系统集成 / Reliability / Delivery

最终进行系统级验证和交付整理，例如：

- 长时间运行；
- Task Stack / Queue / RingBuffer / DMA 资源测量；
- Modbus 压力；
- LVGL 刷新；
- I2C / Flash 资源竞争；
- OTA 故障注入；
- Watchdog；
- Fault / Reset / 掉电恢复；
- 架构与测试文档；
- Release、固件、演示流程和项目总结。

## 3. 路线管理原则

当前可将整个项目粗分为五个方向：

```text
① 工程基础设施
S00 ~ S01

        ↓

② 硬件平台基础
S02 ~ S04

        ↓

③ 核心设备闭环
S05 ~ S07

        ↓

④ 存储与可靠升级
S08 ~ S10

        ↓

⑤ 扩展 + 系统交付
S11 ~ S12
```

路线图用于导航，不作为不可修改的合同。

进入某个阶段前，应重新读取：

- 当前仓库状态；
- 已确认硬件事实；
- 上一阶段输出；
- 测试结果；
- 当前需求和风险。

然后再冻结该阶段自己的设计、实现计划和验收条件。

具体阶段设计、计划、交接和评审仍按 `00_Project/WORKFLOW.md` 管理。
