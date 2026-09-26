# S01 Design

## Metadata

- Stage: `S01 Application 工程初始化与诊断基础`
- Status: `DESIGN_APPROVED`
- Owner: `Project Owner`
- Date: 2026-09-26
- Design Baseline Commit: `fd197f695f0742627470cb54068aadc72a238477`

## Goal

在已经完成 CubeMX 外设与引脚基线的 STM32F407VET6 Application 工程上，建立可长期施工的 Keil 工程、五层架构基础框架和诊断基础设施。

本阶段首先保证：

```text
能稳定 Build
能按规范产生构建输出
能通过 J-Link 下载
能通过 RTT 查看日志
出现 Cortex-M Fault 时能保留并输出诊断现场
CubeMX 再生成后有明确的恢复/检查路径
```

S01 不负责完成 LCD、W25Q128、DHT20、Modbus、OTA、HC-05 等业务功能。

## Inputs and constraints

### 当前工程基线

目标工程：

`03_Firmware/Application/STM32F407_APP/`

当前 CubeMX 基线已经包含：

- STM32F407VET6 / LQFP100；
- FreeRTOS CMSIS-V2；
- TIM2 HAL Timebase；
- SWD；
- SPI1 / W25Q128 引脚；
- USART1 / USART2 / USART3；
- 6 路 UART DMA；
- USART1/2/3 Global IRQ；
- FSMC 16-bit LCD；
- XPT2046 GPIO；
- PB6/PB7 Software I2C GPIO。

尚未确认的硬件事实继续保持 `TO_VERIFY`，不阻塞 S01：

- 实际 HSE 晶振频率；
- PB6/PB7 外部上拉细节；
- HC-05 当前波特率；
- Modbus 正式串口参数；
- RS485 收发方向控制的板级实现。

### 复用资产

优先复用：

`wangyaoqianw-max/Embedded_Engineering_Library`

本阶段设计基线参考 Commit：

`8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0`

主要来源：

- `original/基于五层架构/`
- `original/diagnostics_solution/`
- `adapted/easylogger_rtt_cmsisrtos_port/`
- `third_party/SEGGER_RTT/v7.92/`
- `third_party/EasyLogger/v2.2.99/`
- `third_party/CmBacktrace/v1.5.0/`

### platform_types.h 冻结约束

`original/基于五层架构/03_Platform/platform_common/platform_types.h` 是本项目要求保留的基础资源。

本阶段要求：

- 直接按 Library 当前版本使用；
- 迁入目标工程后不得修改、重构或替换该文件；
- 不因当前代码规范中对标准类型命名的建议而修改它；
- 若实际编译出现冲突，先记录证据并回到设计审查，不允许通过临时改写 `platform_types.h` 绕过问题。

Library 当前该文件 Blob：

`a2d23e8575f31494e55548bde62c15e7407055b9`

## Scope

### In scope

1. Keil 工程与构建输出规范落地。
2. 跑通最基础工具链闭环：Clean Rebuild → 生成固件 → J-Link 烧录 → MCU 启动运行。
3. 建立 Application 五层架构目录与基础公共能力。
4. 选择性复用 Embedded Engineering Library 资产。
5. 接入 SEGGER RTT。
6. 接入 CmBacktrace 与 Cortex-M Fault 诊断链。
7. 接入 EasyLogger、Service Log 和 Platform Log。
8. 建立 CubeMX 再生成后的检查/恢复规则。
9. 执行 Clean Rebuild、基础烧录运行、RTT、Fault 等与本阶段相称的验证。
10. 更新 S01 状态、交接和验证证据。

### Out of scope

- UART Service / RingBuffer 正式通信 Runtime；
- W25Q128 设备驱动；
- DHT20；
- ILI9341/XPT2046 实际驱动；
- LVGL；
- RS485/Modbus；
- OTA/YMODEM；
- HC-05 业务；
- Watchdog Health Policy；
- Bootloader。

这些能力按路线图进入后续阶段。

## Confirmed facts, assumptions, and unknowns

### Confirmed

- CubeMX/Keil Application 工程已经存在。
- 当前 Keil Target 为 STM32F407VETx。
- 根 `.gitignore` 已按目录忽略 `Objects/`、`Listings/` 和普通 `06_Output/`。
- 当前 `.uvprojx` 的 `OutputDirectory` 仍为 `STM32F407_APP\`，`ListingPath` 为空，需要在 S01 修正。
- 当前项目使用 Keil MDK 工程和 STM32CubeF4 HAL。
- Library Diagnostics 方案已有 STM32F4 来源工程复用证据。
- F407/F411 Clean vs Diagnostics 对照实验已经形成 `RTT_CmBacktrace_AI移植指南.md`。

### Assumptions / design choices

- S01 采用 Library 资产的 Copy-in 集成方式，不把 Library 作为运行时 Git Submodule 依赖。
- CubeMX 生成目录保持原位置，不为五层架构移动 `Core/`、`Drivers/`、`Middlewares/`。
- `05_Vendors/` 用于项目主动引入的第三方源码；CubeMX 自带 Vendor 仍保持原目录。
- Fault 路径保持最小依赖，CmBacktrace 不依赖完整 EasyLogger 链。

### Unknown / To Verify

- F407 当前 Fault 回溯是否能在本项目板级通过；
- CubeMX Generate 对本次 Diagnostics 集成的实际覆盖范围；
- EasyLogger Port 在当前 FreeRTOS/CMSIS-V2 配置下是否无需额外调整；
- Keil 工程中与 Fault 汇编、FreeRTOS additions 相关的具体接线是否完全可直接复用。

## Design and module boundaries

### Application 目录

```text
STM32F407_APP/
├── 00_Config/
├── 01_APP/
├── 02_Service/
├── 03_Platform/
├── 04_Impl/
├── 05_Vendors/
├── Core/
├── Drivers/
├── Middlewares/
├── MDK-ARM/
└── STM32F407_APP.ioc
```

依赖方向：

```text
APP
 ↓
Service
 ↓
Platform
 ↓
Impl
 ↓
HAL / RTOS / Vendor / Hardware
```

`00_Config` 是项目静态配置入口，不视为第六业务层。

### S01 最小架构复用

本阶段优先迁入五层架构的基础公共能力，不提前把所有后续模块搬入：

- `platform_common`，包括未修改的 `platform_types.h`；
- `platform_os`；
- `impl_os/freertos`；
- Diagnostics 当前实际依赖的接口。

GPIO/SPI/UART/RingBuffer/UART Service 等功能资产按后续阶段需要再引入，不为了“框架完整”提前扩大 S01。

### 正常日志链

```text
APP / Service
    ↓
service_log
    ↓
platform_log
    ↓
EasyLogger Port
    ↓
EasyLogger
    ↓
SEGGER RTT
```

### Fault 诊断链

```text
HardFault / MemManage / BusFault / UsageFault
                   ↓
        Fault Assembly / Adapter
                   ↓
        diagnostics_fault_handler
             ├───────────────┐
             ↓               ↓
      Cortex-M Context   CmBacktrace
             └───────┬───────┘
                     ↓
                 SEGGER RTT
```

Fault 路径不依赖 EasyLogger Mutex、Task 或异步日志状态。

### Fault Handler 所有权

四个 Cortex-M Fault Handler 必须只有一个活动 Owner。

S01 必须核对：

- startup 弱符号；
- CubeMX 生成的 `stm32f4xx_it.c`；
- CmBacktrace Vendor 示例；
- 项目自有 Fault 汇编。

最终构建中不得存在重复 Handler。

### FreeRTOS 适配

按本仓库 `RTT_CmBacktrace_AI移植指南.md`：

- 优先检查 F407 当前工程是否可使用 FreeRTOS additions 头文件扩展；
- 不默认修改 `tasks.c`；
- 不因移植诊断工具默认增加 Task Stack、FreeRTOS Heap 或 Main Stack；
- 若确实需要变化，先证明原因和配置归属。

## Resource ownership and interactions

- CubeMX 拥有 MCU 外设初始化配置。
- Keil `.uvprojx` 拥有编译 Group、Include Path、输出目录和目标构建配置。
- `05_Vendors` 中第三方源码保持上游风格和版本边界。
- Diagnostics Adapter 拥有 Fault Handler 接管和 RTT 输出桥接。
- Service Log 只提供日志能力，不拥有 RTT/J-Link。
- `platform_types.h` 作为公共类型基础资源保持原样。

## Failure behavior and recovery

- Build 失败先区分源码错误与 Keil 文件 I/O/输出目录问题。
- RTT 不工作时先单独验证 RTT，再排查 EasyLogger。
- Fault Handler 重复定义时不得通过删除随机 Vendor 文件规避，先确定唯一 Owner。
- FreeRTOS 上下文接口不匹配时优先使用项目扩展点，不直接修改 Kernel。
- CubeMX Generate 后出现回退时，根据 Git Diff 检查受影响文件，形成可重复恢复步骤。
- 若必须修改 `platform_types.h` 才能继续，本阶段进入设计复审，不允许直接修改。

## Acceptance criteria

### Basic toolchain

- 当前纯 CubeMX Application 基线可以完成 Clean Rebuild。
- 构建后能够得到可用于烧录的固件映像。
- J-Link 能识别 STM32F407VET6 并完成烧录。
- 烧录后 MCU 能正常启动运行；该验证只证明基础 Build/Flash/Run 链路，不代替任何外设功能验收。
- 实际构建命令、烧录工具/版本、结果和失败信息写入 S01 Verification。

### Code / build

- Keil Target Output Directory = `Objects\`。
- Keil Listing Directory = `Listings\`。
- Clean Rebuild 成功，实际 Error/Warning 数量有记录。
- 构建生成物不污染 Git 工作区。
- 五层基础目录与本阶段复用模块进入 Keil 正确 Group / Include Path。
- `platform_types.h` 与 Library 基线一致。

### Diagnostics

- RTT 独立输出可读。
- EasyLogger 正常日志可通过 RTT 读取。
- Service Log / Platform Log 链路成立。
- CmBacktrace 配置、输出和 Fault Handler Owner 可追溯。
- 在具备硬件条件且经受控启用后，至少完成一种已知 Fault 的现场输出验证；若硬件条件不具备必须明确为 PENDING，不得伪造 PASS。

### Regeneration

- 执行或人工完成一次 CubeMX Generate 后检查关键文件；
- 诊断集成若被覆盖，形成明确恢复步骤；
- 再次 Clean Rebuild 通过。

## Risks and deferred items

- Library `platform_common` 仍有已知技术债，但 S01 不主动重构。
- 第三方版本升级不在本阶段范围。
- UART 多实例资产虽然已存在于 Library，但正式引入推迟到 S04。
- F407 HSE 频率未确认，不影响当前以 HSI 为系统主时钟的 S01。
- 板级 Fault 测试属于破坏性受控测试，只在明确测试模式下执行。

## Required reading

1. `AGENTS.md`
2. `00_Project/WORKFLOW.md`
3. `00_Project/05_Status/current_status.md`
4. `03_Firmware/00_Doc/Keil工程与构建输出规范.md`
5. `03_Firmware/00_Doc/RTT_CmBacktrace_AI移植指南.md`
6. `03_Firmware/00_Doc/嵌入式C代码规范.md`
7. `02_Hardware/Pinout/STM32F407VET6_CubeMX_外设与引脚配置基线.md`
8. Embedded Engineering Library：`8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0`
