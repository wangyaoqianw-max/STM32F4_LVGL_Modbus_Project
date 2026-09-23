# STM32F407VET6 工业控制从机综合项目——工程设计决策

## 设计状态

- 项目类型：Personal Project（个人工业控制方向综合项目）
- 目标 MCU：STM32F407VET6
- 当前阶段：设计基线，可进入工程准备与平台移植阶段
- 设计日期：2026-09-22
- 当前文档只冻结架构、边界、资源所有权和实施顺序，不修改业务代码、不构建、不烧录

## 1. 背景与目标

本项目模拟一个工业控制从机设备。电脑作为 Modbus RTU 主机，通过 RS485 轮询设备；设备侧负责温湿度采集、数据显示、触摸输入、状态维护和协议响应。

项目以两个已有工程为复用来源，再针对 STM32F407VET6 新开发板重新建立板级适配：

1. 复用 `STM32F4_Bootloader_OTA_Test_Project` 中 Bootloader、OTA、外部 Flash、镜像存储、YMODEM 和试运行/确认/回滚思路。
2. 复用 `stm32f4_DMA_UART_ring_RTOS` 中 FreeRTOS 运行基础、DMA UART、环形缓冲、分层组织、传感器采集和测试组织方式。
3. 新开发 ILI9341 FMC 并口显示、XPT2046 模拟 SPI 触摸、LVGL、Modbus RTU 从机、RS485 和新开发板 BSP。
4. 将 HC-05 纳入后续蓝牙串口通道规划；HC-05 是 Bluetooth Classic SPP 模块，不按 BLE/GATT 设备设计。

目标不是把两个 F411 工程目录直接拼接，而是提取稳定的服务和协议边界，在 F407VET6 上重建平台实现。

## 2. 范围与非目标

### 2.1 当前范围

- STM32F407VET6 新板级工程。
- FreeRTOS 应用运行时。
- ILI9341 16 位 8080/FMC 并口显示。
- XPT2046 四线电阻触摸，软件 SPI 读取和校准。
- LVGL 人机界面。
- DHT20 温湿度采集，作为当前可用的传感器实现。
- RS485 半双工链路上的 Modbus RTU 从机。
- `PB6/PB7` 软件 I2C 总线：挂接 `AT24C02` 和 `DHT20`。
- W25Q128 外部 SPI NOR Flash：中文字库、OTA 镜像和大块持久化数据分区。
- AT24C02：保存启动/OTA 元数据等少量关键状态。
- 独立 Bootloader、UART/YMODEM 有线 OTA、镜像安装、试运行、确认和回滚。
- HC-05 蓝牙串口通道的架构预留和后续实施计划。

### 2.2 当前非目标

- 不接入 ESP8266，不设计 Wi-Fi 通信。
- 当前不冻结蓝牙业务协议，也不把 HC-05 OTA 作为第一版验收条件。
- 当前不假设继电器、电机、PWM 执行器或其他控制输出。
- 不在 Bootloader 中加入 LVGL、Modbus、蓝牙、文件系统或复杂网络协议栈。
- 不在 STM32F407VET6 内部 Flash 中放置两份完整 Application；采用外部 Flash staging 保留升级候选镜像。
- 不为未来可能出现的多板卡、多产品型号建立插件系统、动态服务注册中心或全局事件总线。

如果后续加入执行器，新增控制服务和输出平台接口，不改变当前采集、显示、Modbus 和 Bootloader 的核心边界。

## 3. 已验证事实、假设与未知项

### 3.1 已验证事实

#### 用户已确认的需求

- 芯片为 STM32F407VET6，使用新开发板。
- 项目定位为模拟工业控制从机，电脑为 Modbus 主机。
- 传感器暂时使用 DHT20，但传感器类型应可替换。
- 显示控制器为 ILI9341。
- 触摸控制器原理图器件标注为 XPT2046；图纸标题中的“XPT046”按器件标注理解为 XPT2046。
- 外部 Flash 为 W25Q128，已预存中文字库。
- 板上存在 AT24C02；软件 I2C 使用 `PB6=SCL`、`PB7=SDA`，同一总线同时挂接 AT24C02 和 DHT20。
- ESP8266 从当前方案移除。
- HC-05 纳入后续规划。

#### 原理图事实

LCD 母座采用 16 位并行显示接口，连接关系为：

| LCD 信号 | MCU 引脚 |
|---|---|
| `NEx/CS` | `PD7` |
| `NWE/WE` | `PD5` |
| `NOE/RD` | `PD4` |
| `Ax/RS` | `PD11` |
| `FMC_D0/D1` | `PD14/PD15` |
| `FMC_D2/D3` | `PD0/PD1` |
| `FMC_D4~D7` | `PE7~PE10` |
| `FMC_D8~D12` | `PE11~PE15` |
| `FMC_D13~D15` | `PD8~PD10` |
| `LCD_BL` | `PA15` |
| LCD `RESET` | `NRST` 网络 |

触摸控制器采用软件 SPI：

| XPT2046 信号 | MCU 引脚 |
|---|---|
| `T_SCK` | `PE0` |
| `T_MOSI` | `PE2` |
| `T_MISO` | `PE3` |
| `T_PEN` | `PE4` |
| `T_CS` | `PD13` |

`T_PEN` 由 10 kΩ 电阻上拉到 3.3 V。触摸芯片的 `BUSY` 未作为当前接口使用。

LCD 母座由 5 V 输入经 ME6211C33M5G 产生 3.3 V_LCD，MCU 侧接口按 3.3 V 逻辑设计。

#### 软件 I2C 总线事实

当前确认软件 I2C 使用：

| 信号 | MCU 引脚 |
|---|---|
| `SCL` | `PB6` |
| `SDA` | `PB7` |

该总线同时挂接 `AT24C02` 和 `DHT20`。两者的器件地址、上拉阻值、电气有效电平和上电时序仍需根据实际原理图与器件资料复核；软件上按共享总线设计，不允许两个服务并发直接驱动 GPIO。

#### MCU 资源基线

STMicroelectronics 对 STM32F407VE 给出的资源基线为 512 Kbytes Flash、最高 168 MHz、192+4 Kbytes SRAM（含 64 Kbytes CCM Data RAM），并带有 FSMC 静态存储控制器、并行 LCD 接口和多路 USART/UART、SPI、I2C、DMA 资源。最终仍以实际芯片数据手册、封装引脚复核和 CubeMX 资源检查为准。

### 3.2 项目假设

- 先沿用来源工程的 Keil MDK、STM32CubeMX/HAL 和 FreeRTOS 工具链；若后续改用其他工具链，只替换工程实现层，不改变应用服务边界。
- 当前 OTA 保留为电脑通过独立 UART 使用 YMODEM 传输；RS485 UART 专门用于 Modbus，不与 OTA 共用。
- HC-05 使用独立 UART，第一阶段只完成通道和基础收发规划，不依赖它完成系统启动、Modbus 或 OTA。
- W25Q128 承载中文字库、OTA 候选镜像和大块持久化数据；AT24C02 承载少量启动/OTA 元数据。两类存储必须分别建立逻辑访问边界，禁止模块直接使用裸地址。
- `PB6/PB7` 软件 I2C 是 DHT20 与 AT24C02 的共享资源；需要事务级互斥、超时和总线恢复策略。
- DHT20 只实现为 `Sensor` 契约下的一个具体驱动；上层使用统一的温湿度样本和质量状态。
- 当前系统重点是采集、显示和从机通信，不构成实际闭环控制系统。

### 3.3 工程准备阶段必须关闭的项目

这些项目不会改变总体分层，但必须在编码和 linker 冻结前确认：

- ILI9341 屏幕的实际分辨率、方向、时序、背光有效电平和初始化序列。
- `NRST` 与 LCD `RESET` 共用的硬件意图；当前连接意味着软件不能独立复位 LCD。
- W25Q128 的实际 JEDEC ID、容量、页大小、扇区大小、接线和 SPI 资源。
- RS485 收发器的 UART、`DE/RE` 方向控制脚和失效安全电平。
- OTA UART、HC-05 UART、Modbus UART 的具体分配，避免 DMA Stream/Channel 冲突。
- AT24C02/DHT20 的器件地址、软件 I2C 上拉条件、总线速率和异常恢复边界。
- AT24C02 元数据记录尺寸、写周期等待方式和掉电写入测试条件。
- Modbus 从站地址、波特率、校验方式、寄存器地址和异常码约定。
- 中文字库格式、起始地址、大小和 LVGL 字体访问方式。

## 4. 来源工程复用决策

### 4.1 复用矩阵

| 来源 | 可复用内容 | 复用方式 | 不直接复用的内容 |
|---|---|---|---|
| `STM32F4_Bootloader_OTA_Test_Project` | Bootloader/APP 分离、外部 Flash 镜像、YMODEM、CRC、Pending/Trial/Confirm/Rollback、工具和验证思路 | 复用状态语义、数据契约和经过核对的模块实现 | F411 启动文件、Flash 地址、linker、F411 引脚、W25Q64/AT24C02 板级代码 |
| 来源工程 Application 的 `app_system`/`app_startup` 组合 | 启动上下文、组件启动结果、事件屏障、长期任务启动和 RUNNING/DEGRADED/FAILED 裁决 | 复用启动编排思路和接口语义，改为本工程的 `appSystemTask`；保留本工程的 `defaultTask` 约束 | 原工程的 F411 任务参数、组件清单、硬件初始化和 `defaultTask` 自删除行为 |
| `stm32f4_DMA_UART_ring_RTOS` | FreeRTOS 运行时、DMA UART、ring buffer、App/Service/Platform/Impl 组织、DHT20 采集思路、Host/Keil/板级验证结构 | 移植通用算法和窄接口，再重新绑定 F407 BSP | F411 CubeMX 工程、启动文件、F411 DMA 映射、ST7789 驱动、原开发板引脚 |
| 来源工程的显示部分 | 显示任务、LVGL 生命周期、队列和状态模型 | 只复用运行模型和测试经验 | ST7789 SPI 总线、原屏幕初始化、原分辨率和原触摸实现 |

两个来源仓库的 GitHub 仓库元数据当前未声明明确 License。直接复制代码前应保留来源、提交版本和文件归属记录，并确认个人学习使用范围；不能把“公开仓库”自动当作“无许可限制”。

### 4.2 复用边界

复用按三类处理：

1. **协议/算法层**：可以先移植，再通过 Host 或板级测试验证，例如环形缓冲、YMODEM、CRC、OTA 状态转换。
2. **服务层**：可以参考目录和职责，但要重新检查依赖，例如采集服务、显示任务、OTA 服务、日志服务。
3. **板级/芯片层**：不直接复制，例如启动文件、时钟、GPIO、FMC、DMA、UART、SPI、I2C、linker 和屏幕驱动。

## 5. 总体架构决策

### 5.1 分层

```text
Bootloader
├── Boot decision / AT24C02 metadata / image validation
├── W25Q128 minimal access
├── AT24C02 minimal software-I2C access
├── Internal Flash installation
├── Trial / Confirm / Rollback
└── Jump to Application

Application
├── App：设备状态、数据模型、应用策略
├── Service：采集、显示、Modbus、OTA、存储、可选蓝牙
├── Platform：UART/RS485、I2C、FMC LCD、Touch SPI、SPI Flash、OS
├── Impl：STM32F407VET6 新板卡具体实现
└── Vendor/HAL：STM32 HAL、FreeRTOS、LVGL、第三方库
```

依赖方向固定为：

```text
App → Service → Platform → Impl → HAL/Vendor
```

Application 不包含 `HAL_*` 类型，Service 不直接访问 GPIO、DMA、FMC 或具体 UART 句柄。Bootloader 是独立的小工程，不复制 Application 的完整五层结构。

### 5.2 子系统复杂度选择

| 子系统 | 复杂度 | 决策依据 |
|---|---:|---|
| Bootloader/OTA | L2 | 具有独立启动边界、掉电风险、镜像状态和恢复路径；需要独立于 Application 验证 |
| Application Runtime | L2 | 显示、采集、Modbus、OTA 具有不同阻塞域和执行上下文，需要有限的多任务和 IPC |
| Display/LVGL | L1 | 显示接口与业务变化独立，但当前只有一种 LCD，采用窄显示平台接口，不建立通用显示插件框架 |
| Sensor | L1 | 传感器确实存在替换轴，但当前只需 DHT20 一个实现；使用小型 `sensor_ops`，不做运行时注册中心 |
| Modbus/RS485 | L2 | UART DMA、帧边界、3.5 字符静默时间、半双工方向和从机响应需要单一 owner task |
| Shared Software I2C | L1 | AT24C02 与 DHT20 共用 `PB6/PB7`，需要事务级互斥和总线恢复；不引入通用 I2C 设备框架 |
| AT24C02 Boot Metadata | L1 | 容量小、写入频率低，只保存启动/OTA 状态双副本；不保存日志和大块数据 |
| W25Q128 Storage | L1 | 字库、OTA 镜像和大块数据共享一个外部 Flash，需要统一分区和访问边界；不引入文件系统 |
| HC-05 | L1（规划） | 作为可选 UART 通道接入；当前不让它改变核心启动、采集或 Modbus 结构 |

不采用 L3 平台化架构：当前只有一个个人项目、一个 MCU 型号和一块新板，没有多产品族、多人并行或长期兼容接口的证据。

## 6. 模块边界与关键接口

### 6.1 Application 与 Service

| 模块 | 唯一负责 | 不负责 | 主要依赖 |
|---|---|---|---|
| `app_system` | 启动编排、设备状态、样本快照、系统错误状态 | 不直接读 HAL，不管理具体协议帧 | Service 接口、RTOS 平台接口 |
| `service_acquisition` | 触发采集、校验样本、生成质量状态 | 不绘制 UI，不解析 Modbus | `sensor_ops`、I2C Platform |
| `service_modbus` | Modbus RTU 帧解析、CRC、寄存器映射、异常响应 | 不直接访问 DHT20 和 LCD | RS485/UART Platform、只读数据快照 |
| `service_display` | LVGL 页面、状态和样本呈现、触摸事件 | 不被其他任务直接调用 LVGL | LCD Platform、Touch Platform、显示队列 |
| `service_ota` | YMODEM 会话、候选镜像接收、校验、升级请求 | 不决定最终启动，不在当前 APP 内覆盖运行区 | OTA UART、W25Q128、AT24C02 元数据、Boot Contract |
| `service_boot_metadata` | AT24C02 双副本元数据、序列号、提交标记和升级状态 | 不保存日志/镜像，不理解 UI 和 Modbus 业务 | 软件 I2C Platform、AT24C02 Impl |
| `service_external_flash` | W25Q128 分区、读写、校验、字体/镜像/大块数据访问 | 不保存启动元数据，不理解 UI 和 Modbus 业务 | SPI Flash Platform |
| `service_bluetooth` | 后续 HC-05 串口接入、透明数据或诊断命令 | 当前不成为系统必需依赖，不默认拥有 OTA 权限 | 独立 UART Platform |

### 6.2 传感器契约

DHT20 作为具体实现，向采集服务提供最小契约：

```text
sensor_init()
sensor_start_measurement()
sensor_read(sample)
sensor_get_status()
```

采集结果统一为：

```text
temperature
humidity
timestamp
validity/status
sequence
```

上层不判断“这是 DHT20 还是其他传感器”。将来更换 ADC、SPI、I2C 或 Modbus 传感器时，只替换驱动和适配层。

### 6.3 Modbus 契约

Modbus RTU 从机只在收到主机请求后响应。初版寄存器模型至少应能表达：

- 温度、湿度及其有效性；
- 设备运行状态、传感器错误和通信错误计数；
- 固件版本、Bootloader/OTA 状态；
- 后续控制寄存器的保留区，但不虚构具体执行器。

寄存器地址、数据缩放、读写权限和异常码在工程准备阶段冻结。Modbus 服务只读取应用快照，不直接读 DHT20；这样主机轮询不会阻塞传感器采集。

### 6.4 OTA 与 Bootloader 契约

当前采用：

```text
PC
  ↓ 独立 UART + YMODEM
Application OTA Service
  ↓ 分块写入
W25Q128 OTA 区域
  ↓ 镜像头/CRC 校验
AT24C02 双副本元数据
  Pending
  ↓ Reset
Bootloader
  ↓ 读取 AT24C02 状态，再验证 W25Q128 镜像、安装、Trial
Application Confirm 或 Rollback
```

Bootloader 与 Application 只共享最小契约：

- 镜像头和版本信息；
- W25Q128 逻辑分区格式；
- AT24C02 元数据记录格式和提交语义；
- `request_upgrade`、`confirm_running_image` 等最小操作语义。

当前个人项目基线使用 CRC32 做传输和存储错误检测；CRC32 不等于数字签名，也不构成安全启动。数字签名、Secure Boot 和 anti-rollback 作为后续安全增强，不混入当前第一版实现。

## 7. 运行模型、任务与 IPC

### 7.1 任务划分

任务只按独立 deadline、阻塞域、硬件所有权或故障隔离建立，不按每个文件机械建立任务。

| 任务 | 触发 | 主要所有权 | 初始职责 |
|---|---|---|---|
| `defaultTask` | CubeMX 生成，调度器启动后常驻 | CubeMX 默认任务上下文 | 保留为工程入口/空闲等待任务，不承担业务初始化，不访问具体外设 |
| `appSystemTask` | FreeRTOS 调度器启动后一次执行 | `app_system` 初始化上下文 | 创建 RTOS 对象、按依赖顺序组织初始化、创建长期任务、发布就绪/降级状态，完成后自删除 |
| `appTask` | 事件/周期 | 应用状态和样本快照 | 汇总采集结果、更新状态、发布 UI/通信可见快照 |
| `acquisitionTask` | 固定周期 | DHT20 采集流程 | 通过共享软件 I2C 总线采样、质量检查、发布 `sensor_sample_t` |
| `modbusTask` | UART DMA/IDLE 通知 | RS485 UART、DE/RE、协议状态 | 接收帧、静默超时判帧、解析请求、生成响应 |
| `displayTask` | LVGL tick/显示队列/触摸事件 | LVGL、FMC LCD、XPT2046 | 初始化 LCD、运行 LVGL、处理触摸、消费最新状态 |
| `otaTask` | UART/YMODEM 事件 | OTA UART、OTA 会话 | 接收、重试、分块写 W25Q128、校验、更新 AT24C02 元数据并请求升级 |
| `bluetoothTask` | 后续规划 | HC-05 UART | 仅在 HC-05 功能启用时创建；不影响当前系统启动 |

初版长期任务控制在 5 个左右（不含可选蓝牙任务），优先级只按通信响应、采集周期、UI 响应和后台 OTA 的实际测量调整，不预先堆高优先级。

### 7.1.1 `defaultTask` 与 `appSystemTask`

启动阶段分为“调度器前的最小硬件准备”和“调度器后的 `appSystemTask` 编排”两段，组织方式参考来源工程 Application 的 `app_system_bootstrap()`、`app_startup` 上下文和组件启动屏障：

1. **调度器前**：在 `main`/CubeMX 生成的入口中只完成 `HAL_Init`、时钟、基础 GPIO、DMA/中断前置配置和 FreeRTOS 启动对象准备；不在这里编写长期业务初始化流程。
2. **保留 `defaultTask`**：它是 CubeMX 生成的默认任务，按当前工程约束不删除、不改造成业务任务；启动后进入阻塞/低频等待，不直接初始化 LCD、传感器、Modbus 或 OTA。
3. **初始化启动上下文**：`appSystemTask` 先创建或确认 `i2c_bus_mutex`、样本/显示队列、事件组、UART 通知关联和系统状态对象，并建立类似 `app_startup_context` 的组件结果记录，确保后续任务不会访问未准备好的 IPC 对象。
4. **启动组件任务**：基础对象完成后，按应用组合层统一调用各组件的 `*_task_start()`，创建 `appTask`、`acquisitionTask`、`modbusTask`、`displayTask`、`otaTask` 以及按配置启用的 `bluetoothTask`。各长期任务在自己的上下文中完成 LCD/LVGL、RS485、DHT20 采集流程和 UART 会话等运行时初始化，并回报自己的初始化结果。
5. **等待启动屏障**：`appSystemTask` 使用事件组等待所有核心组件在有限超时时间内报告 `READY/FAILED`，然后根据结果形成 `SYSTEM_READY`、`SYSTEM_DEGRADED` 或 `SYSTEM_FAILED` 裁决。AT24C02 写周期、DHT20 转换和其他可能阻塞的操作必须使用超时，不能让启动任务无限等待。
6. **发布结果并退出**：发布系统启动裁决和初始设备状态；非关键外设失败时允许系统带降级能力运行，关键 RTOS 对象或启动契约失败时进入最小故障路径。正常路径最后调用 `vTaskDelete(NULL)`，不保留空转循环。

`appSystemTask` 是一次性启动任务，不调用 LVGL、不解析 Modbus/YMODEM、不长期持有软件 I2C 锁，也不在删除自身后继续拥有任何外设。`app_system` 模块负责启动编排、设备状态和应用状态初始化；长期任务负责各自运行时资源的实际使用。

工程配置上，在 CubeMX 的 FreeRTOS 线程列表中同时保留 `defaultTask` 并新增 `appSystemTask`；由 RTOS 在调度器启动前创建两者，避免把 `appSystemTask` 的创建责任再放回不可删除的 `defaultTask`。

### 7.2 IPC 与数据流

```text
acquisitionTask → sensor service ───────┐
                                       ├→ i2c_bus_mutex → soft I2C PB6/PB7 → DHT20
otaTask → service_boot_metadata ────────┘                         └→ AT24C02

sensor service → sensor_sample_queue → appTask / Data Snapshot
    ├── display_queue → displayTask → LVGL → ILI9341/FMC
    └── read-only snapshot → modbusTask → RS485 → PC Master

RS485 UART DMA/IDLE → ISR → notification → modbusTask
OTA UART DMA/IDLE    → ISR → notification → otaTask
HC-05 UART（未来）   → ISR → notification → bluetoothTask

otaTask → service_external_flash → W25Q128
otaTask → service_boot_metadata → AT24C02
Bootloader → W25Q128 + AT24C02 minimal drivers → image validation/install
```

IPC 选择：

- UART DMA/IDLE 到单一任务：Task Notification。
- 传递固定大小的样本或状态：Queue；队列满时按“保留最新样本”或显式报错处理。
- 显示：只传递 `SYSTEM_STATE`、`MEASUREMENT` 等小型值对象，Display Task 内合并旧状态，禁止多个任务直接调用 LVGL。
- 多条件启动状态：Event Group 或明确的启动状态机。
- 软件 I2C：由 `i2c_bus_mutex` 保护完整的单次事务；DHT20 转换等待和 AT24C02 内部写周期等待期间释放总线锁，重新访问时使用有界超时。任何 ISR 不直接访问该总线。
- AT24C02：由 `service_boot_metadata` 统一访问，只写双副本元数据，不承载日志、字库或镜像；写后回读校验并等待内部写周期完成。
- W25Q128：由 `service_external_flash` 统一访问；每次操作为有上界的读、页写或扇区操作，OTA 大传输分块释放资源。
- 软件 I2C 发生 SDA 被拉低、超时或异常复位后，平台层提供 GPIO 恢复、STOP 和重新初始化路径，并记录总线错误。
- 不建立全局 Event Bus，不把每条数据都包装成动态 Command 对象。

### 7.3 中断原则

- ISR 只处理 DMA/IDLE 标志、保存必要的硬件状态和发送 FromISR 通知。
- Modbus 帧解析、CRC、YMODEM、Flash 写入、LVGL 和传感器转换全部延后到任务上下文。
- DMA 缓冲区放在 DMA 可访问的 SRAM 区域，不能仅凭普通 RAM 地址假设 CCM 可被所有 DMA 路径访问。
- UART 调试日志不能混入 Modbus、OTA 或 HC-05 数据通道。

## 8. 硬件与资源规划

### 8.1 LCD 与触摸

LCD 采用 F407 的 FSMC/FMC 静态存储控制器映射，软件上拆为：

```text
ili9341_bus_fmc
    ├── command write/read
    ├── data write/read
    ├── timing configuration
    └── backlight control

ili9341_driver
    ├── reset/init sequence
    ├── orientation/color format
    ├── window/partial flush
    └── LVGL flush adapter
```

XPT2046 单独作为触摸输入平台：

```text
xpt2046_sw_spi
    ├── raw ADC read
    ├── PENIRQ handling
    ├── calibration
    ├── pressure/threshold filtering
    └── LVGL input adapter
```

默认采用 LVGL 部分刷新和有限大小 draw buffer，不申请完整屏幕 framebuffer；实际 draw buffer 大小在分辨率、刷新速度和 RAM map 确认后测量确定。

原理图中 LCD `RESET` 接系统 `NRST`，因此当前设计的显示故障恢复只能使用重新初始化或系统复位。若后续板卡可修改，优先增加独立 LCD reset GPIO；若硬件不变，则在风险记录中明确该限制。

### 8.2 共享软件 I2C 总线与 AT24C02

`PB6/PB7` 不是某个单一设备的私有接口，而是 DHT20 与 AT24C02 共用的软件 I2C 总线：

```text
PB6 = SCL
PB7 = SDA
    │
    ├── DHT20       采集设备
    └── AT24C02     启动/OTA 元数据
```

平台层提供一个共享 `soft_i2c_bus`，上层设备不能直接操作 GPIO。总线规则如下：

- 以一次完整 I2C 事务为互斥边界，由 `i2c_bus_mutex` 保护 `START → 地址/读写 → STOP`。
- DHT20 发起测量后释放总线锁，在转换等待期间不占用总线；读取结果时重新加锁并使用有界超时。
- AT24C02 页写后释放总线锁，内部写周期完成后再轮询或重试；不能长时间持锁等待 EEPROM 完成。
- 任何 ISR 不访问软件 I2C；采集任务和元数据服务都在任务上下文执行。
- SDA/SCL 超时或被拉低时，平台层执行有限次数的 SCL 脉冲、STOP 和 GPIO/软件 I2C 重初始化，并上报总线错误。

AT24C02 容量只有少量字节的存储空间，职责限定为启动/OTA 元数据，不保存日志、字库或镜像。元数据采用双副本记录：

```text
写 inactive record
→ 回读校验
→ 写 commit marker
→ 启动时选择 CRC 正确且 sequence 最大者
```

记录至少需要容纳：活动/候选槽位、镜像版本、长度、CRC、状态、安装 checkpoint、试运行次数、sequence、commit marker 和记录 CRC。具体记录尺寸、器件地址、页边界和写周期必须在工程准备阶段按实际器件资料冻结。推荐状态：`EMPTY`、`DOWNLOADING`、`VERIFIED`、`PENDING`、`INSTALLING`、`TRIAL`、`CONFIRMED`、`ROLLBACK`、`FAILED`。

### 8.3 W25Q128

W25Q128 由 `service_external_flash` 管理，Bootloader 只保留最小读、状态检查和镜像访问能力，建议逻辑分区如下：

```text
W25Q128
├── Font Region        预存中文字库，只读为主
├── OTA Slot A         候选/历史镜像
├── OTA Slot B         候选/历史镜像
├── Config/Log         后续参数和诊断记录
└── Reserved           对齐和后续扩展
```

W25Q128 不保存启动元数据；启动/OTA 状态的权威双副本位于 AT24C02。W25Q 镜像自身仍应包含 magic、版本、长度和 CRC 等自描述信息，用于接收完成、Bootloader 安装前和安装后的独立校验，但不能替代 AT24C02 的状态提交语义。

实际地址不能在当前文档中凭空冻结，必须先测量：最大 Application 镜像、字库大小、配置/日志预算、W25Q128 JEDEC 几何和保留寿命。分区表一旦冻结，Application 与 Bootloader 必须共同使用同一份只读契约。

### 8.4 内部 Flash

由于 F407VET6 只有 512 Kbytes Flash，且 Application 将包含 FreeRTOS、LVGL、Modbus、OTA 和显示驱动，初版采用：

```text
Internal Flash
├── Bootloader
├── 单份当前运行 Application
└── 保留的配置/安装状态区域（最终按 Flash sector 冻结）
```

外部 W25Q128 负责升级候选镜像和回滚镜像，避免在内部 Flash 上强行放置两个完整 LVGL Application。Bootloader 安装时按 sector 分步复制，每一步都能在复位后判断进度；安装完成后对内部 Application 再次校验。

### 8.5 RAM 与动态内存

- RTOS 任务、Queue、DMA ring buffer 和 LVGL draw buffer 优先采用静态或启动阶段分配。
- 运行期控制路径不使用不可预测的大块动态分配。
- 保留 `configASSERT`、栈溢出检查、malloc 失败钩子和运行时 stack high-water 观测。
- 以 linker map、任务栈余量、Queue 峰值和最小 heap 余量作为资源验收证据。

## 9. 可靠性、错误与降级策略

| 故障 | 预期处理 |
|---|---|
| DHT20 无响应/校验失败 | 保留上次样本并标记无效/过期；UI 告警；Modbus 返回状态位，不拖死其他任务 |
| 软件 I2C 总线卡死/超时 | 结束当前事务，执行有限总线恢复和重新初始化；DHT20 样本标记无效，AT24C02 元数据操作失败时禁止推进 OTA 状态 |
| AT24C02 读写/双副本均失败 | 保守停留在当前已确认 Application；不把 W25Q 镜像直接标记为可安装；记录元数据故障 |
| Modbus CRC/长度/静默时间错误 | 丢弃当前帧、计数并等待下一帧；不因单帧错误重启 |
| RS485 方向或发送超时 | 释放总线状态、记录错误并回到接收态 |
| ILI9341 初始化失败 | Display 进入 DEGRADED；采集、Modbus 和日志继续运行 |
| XPT2046 读数异常 | 禁用当前触摸事件，显示和 Modbus 不受影响 |
| W25Q128 JEDEC/读写失败 | 禁止 OTA 状态推进；中文字库读取失败时使用可用的降级字体/页面；保留当前 Application |
| OTA 传输中断或 CRC 错误 | 清除未完成候选状态，不覆盖当前运行镜像 |
| Bootloader 安装中断 | 根据 AT24C02 元数据和安装 checkpoint 恢复、重试或回滚，不直接跳转未知镜像 |
| Trial Application 看门狗复位 | Bootloader 识别 Trial 失败，达到策略次数后恢复上一 Confirmed 镜像 |
| HC-05 缺失或断开 | 蓝牙功能降级，不影响采集、显示、Modbus 和本地 OTA |

IWDG 作为 Trial Boot 和系统失控保护手段。Watchdog 不能只由一个可能已经卡死的任务自行证明健康；需要由关键任务状态汇报或系统级进度监测决定是否喂狗。

## 10. 不采用的设计及理由

### 10.1 不直接复制 F411 整个工程

来源工程的启动文件、CubeMX 初始化、DMA 映射、Flash 地址、外设句柄和屏幕硬件都绑定 F411/旧板。直接复制会把旧板事实伪装成新板事实，后期问题难以定位。

### 10.2 不在内部 Flash 做完整 A/B

F407VET6 的内部 Flash 容量需要同时容纳 Bootloader 和较大的 LVGL Application。外部 W25Q128 已经提供 staging 空间，因此采用外部 A/B 镜像 + 内部单一运行镜像，复杂度和可靠性更平衡。

### 10.3 不建立通用显示插件或传感器注册中心

当前只有 ILI9341/XPT2046 和 DHT20 一个具体实现。显示使用窄平台接口，传感器使用静态 `sensor_ops`；等真实出现第二种实现且修改传播成本明显增加时再扩展。

### 10.4 不为每个功能建立一个 Task

LVGL、Modbus、采集和 OTA 各自存在阻塞域或资源所有权依据；但字体、数据模型、CRC 和状态转换仍使用函数/模块，不单独创建任务。

### 10.5 不让 HC-05、Modbus、OTA 共用一个实时 UART

三者的帧边界、时序、错误恢复和数据所有权不同。若物理资源不足，应减少后续可选功能，而不是在一个 UART 上混合三种协议。

## 11. 风险与验证项

### 11.1 主要风险

1. **FMC 引脚占用大**：ILI9341 16 位并口占用大量 GPIOD/GPIOE 引脚，需要在完整原理图上重新核对 W25Q、RS485、UART、I2C、SWD 和调试口是否冲突。
2. **LCD 无独立 Reset**：`NRST` 共用会降低显示模块的独立恢复能力。
3. **共享软件 I2C 竞争**：DHT20 与 AT24C02 共用 `PB6/PB7`，必须通过事务级互斥、超时和总线恢复避免互相阻塞。
4. **W25Q128 多用途竞争**：字库读取、OTA 写入和配置/日志访问必须通过统一分区与访问服务隔离；启动元数据不放入 W25Q128。
5. **OTA 可靠性不是 CRC 就能证明**：必须验证每个安装状态的复位/掉电行为；当前 CRC 只解决错误检测，不解决来源认证。
6. **Modbus 数据模型尚未冻结**：寄存器地址和缩放一旦对外发布就会形成兼容性约束，必须在编码前形成版本化表格。
7. **来源工程与目标工程芯片不同**：至少要重新验证时钟、NVIC、DMA、缓存/缓冲区地址、Flash sector 和 linker。
8. **中文字库占用与 LVGL RAM/Flash 预算未知**：必须先确认字体格式和访问方式，再确定缓存策略。

### 11.2 验证项

平台层：

- F407VET6 时钟、复位、SWD、Heap/Stack、FreeRTOS 启动。
- FMC/FSMC 16 位 LCD 读写时序、ILI9341 ID、颜色、方向和窗口刷新。
- XPT2046 原始坐标、PENIRQ、校准矩阵、抖动过滤和 LVGL 输入事件。
- `PB6/PB7` 软件 I2C 空闲电平、起停时序、设备寻址、锁竞争、超时和总线恢复。
- AT24C02 页写、写周期、回读、掉电后的 Metadata 双副本选择和寿命边界。
- W25Q128 JEDEC ID、页写、扇区擦除、回读和分区边界。
- 每个 UART 的 DMA Stream/Channel、中断优先级和 ring buffer 可访问内存。

功能层：

- DHT20 周期采集、CRC/状态、断线降级和数据快照。
- PC Modbus 主机轮询、正常响应、CRC 错误、非法地址、超时和异常码。
- LVGL 显示温湿度、通信状态、传感器状态和 OTA 状态。
- UART/YMODEM 接收中断、CRC 错误、候选镜像写入和 Pending 状态。
- Bootloader 安装、Trial、Confirm、Watchdog Reset、Rollback。
- HC-05 接入后独立收发和断开恢复，不影响 Modbus。

资源与可靠性层：

- 所有长期任务的栈 high-water mark。
- Queue 峰值、UART ring 溢出、Modbus 响应最坏延迟。
- LVGL 刷新时间、触摸读取耗时、软件 I2C 事务耗时和 W25Q128 访问阻塞时间。
- OTA 在下载完成前、Pending 前、安装每个 sector 后和 Confirm 前复位/掉电。
- 当前 Application、候选镜像、AT24C02 Metadata A/B 同时出现损坏时的安全状态。

## 12. 分阶段实施路线

### 阶段 0：工程准备与硬件事实基线

**输入**：完整原理图、Pinout、ILI9341/XPT2046/W25Q128/AT24C02/DHT20/RS485/HC-05 资料、Keil/CubeMX/调试器版本。

**输出**：F407VET6 外设资源表、DMA/IRQ 表、Flash 初步容量表、`PB6/PB7` 软件 I2C 设备表、W25Q128 分区草案、AT24C02 元数据草案、Modbus 寄存器草案、来源代码版本和许可记录。

**进入下一阶段条件**：无未解释的引脚冲突；LCD/FMC、W25Q、`PB6/PB7` 共享软件 I2C、RS485、OTA UART、HC-05 UART 的物理连接可定位。

### 阶段 1：F407VET6 最小平台

**输入**：阶段 0 资源表。

**实施**：新建 F407 工程；完成时钟、启动文件、linker、GPIO、SWD、RTT、FreeRTOS、保留的 `defaultTask`、一次性 `appSystemTask` 和基础错误钩子。

**验收输出**：最小 Application 可启动；`appSystemTask` 能按顺序创建基础对象和长期任务、发布就绪/降级状态并自删除，`defaultTask` 保持阻塞/等待；系统能报告芯片/版本/复位原因，资源 map 可读。

### 阶段 2：运行时与 DMA UART 基线

**输入**：F407 平台和来源工程 UART/ring 代码。

**实施**：移植 UART DMA + IDLE + ring buffer；先打通一条测试 UART，再分别绑定 Modbus UART、OTA UART 和后续 HC-05 UART。

**验收输出**：无丢帧/可观测溢出/中断到任务通知链路；Host 和目标板均能验证。

### 阶段 3：LCD、触摸与 LVGL

**输入**：FMC 引脚表、ILI9341 屏幕资料、XPT2046 原理图。

**实施**：FMC 总线、ILI9341 初始化和局部刷新；XPT2046 软件 SPI、PENIRQ、校准；单一 Display Task 接入 LVGL。

**验收输出**：显示颜色/方向/刷新正确，触摸坐标经过校准，LVGL 只在 Display Task 运行。

### 阶段 4：可替换传感器采集

**输入**：`PB6/PB7` 共享软件 I2C、DHT20 资源和 `sensor_ops`。

**实施**：软件 I2C 总线、`i2c_bus_mutex`、DHT20 驱动、采集周期、质量状态、样本队列和应用快照；验证 DHT20 转换等待不长期占用总线。

**验收输出**：DHT20 正常/异常均不会阻塞 Modbus 和显示；AT24C02 同时访问时无 GPIO 竞争；上层不出现 DHT20 专用判断。

### 阶段 5：Modbus RTU 从机

**输入**：RS485 方向控制和寄存器表。

**实施**：帧边界、CRC、从机地址过滤、寄存器读写、异常码、错误计数和响应超时。

**验收输出**：电脑作为主机可以稳定读取温湿度、状态和版本；非法请求行为明确。

### 阶段 6：AT24C02/W25Q128 外部存储与中文字库

**输入**：AT24C02/W25Q128 datasheet、`PB6/PB7` 软件 I2C 设备资料、字体文件和镜像容量估算。

**实施**：AT24C02 双副本元数据、W25Q128 SPI Flash 驱动和分区表、字库读取/缓存、共享软件 I2C 事务互斥、写周期处理和读回验证。

**验收输出**：AT24C02 元数据掉电选择正确；W25Q128 JEDEC/读写/擦除正常；中文字库可由 LVGL 读取，OTA 区域与字体区域无重叠。

### 阶段 7：Bootloader 与 UART OTA

**输入**：阶段 2、6、AT24C02 元数据契约和内部 Flash/linker 基线。

**实施**：最小 Bootloader、镜像头、CRC、YMODEM 候选接收、Pending、安装 checkpoint、Trial、Confirm、Watchdog 和 Rollback。

**验收输出**：正常升级、传输中断、镜像损坏、安装复位和 Trial 失败回滚均有证据；Bootloader 不依赖 LVGL/FreeRTOS。

### 阶段 8：HC-05 蓝牙通道

**输入**：独立 UART 资源和明确的蓝牙业务协议。

**实施**：HC-05 上电/波特率/连接状态、串口收发和最小诊断或参数通道。若后续要求蓝牙 OTA，复用 OTA session/transport 契约，不在 HC-05 模块内另造一套升级状态机。

**验收输出**：HC-05 插拔/断连不影响核心系统；蓝牙功能有明确权限和超时边界。

### 阶段 9：系统级验证

**输入**：全部模块和资源报告。

**实施**：Host 单元测试、目标板集成测试、Modbus 长时间轮询、显示/触摸压力、软件 I2C 设备访问竞争、W25Q128 访问竞争、OTA 故障注入和资源测量。

**完成条件**：架构边界未被绕过；任务栈、Queue、DMA ring、Flash 分区和 OTA 状态机都有可回读证据；设计阶段停止，不在本阶段文档中代替构建/烧录/调试工作。

## 13. 设计结论

1. 这是一个个人工业控制从机综合项目，采用有限的 L2 分层和多任务运行模型，不做产品平台化。
2. F407VET6 新板上的 BSP、FMC/ILI9341、XPT2046、DMA/IRQ、linker 和 Flash map 必须重新建立。
3. DHT20 只是可替换传感器接口下的第一个实现，不进入上层业务语义。
4. Modbus RTU 从机、RS485 和电脑主机轮询是当前核心通信闭环。
5. AT24C02 保存启动/OTA 双副本元数据，W25Q128 承载中文字库、OTA 候选镜像和大块数据；两者分别通过元数据服务和外部 Flash 服务管理。
6. 当前 OTA 采用有线 UART/YMODEM；HC-05 先进入可选蓝牙串口规划，蓝牙 OTA 不属于第一版验收。
7. Bootloader 保持最小、独立和可回滚；Application 负责传输和业务策略，Bootloader 负责最终镜像验证与安装。
8. 当前不加入未确认的执行器和控制输出；后续增加执行器时沿用现有数据模型和服务边界扩展。
9. FreeRTOS 保留 CubeMX 的 `defaultTask` 作为非业务等待任务，新增一次性 `appSystemTask` 作为启动编排器；长期任务只承担各自的运行时所有权。

## 14. 参考资料

- [STM32F407VE 产品页](https://www.st.com/en/microcontrollers-microprocessors/stm32f407ve.html)
- [STM32F405/407 数据手册](https://www.st.com/resource/en/datasheet/stm32f407ve.pdf)
- [STM32F4_Bootloader_OTA_Test_Project](https://github.com/wangyaoqianw-max/STM32F4_Bootloader_OTA_Test_Project)
- [来源工程 `freertos.c`](https://github.com/wangyaoqianw-max/STM32F4_Bootloader_OTA_Test_Project/blob/main/03_Firmware/Application/OTA_APP/Core/Src/freertos.c)
- [来源工程 `app_system.c`](https://github.com/wangyaoqianw-max/STM32F4_Bootloader_OTA_Test_Project/blob/main/03_Firmware/Application/OTA_APP/01_APP/system/app_system.c)
- [来源工程 `app_startup.c`](https://github.com/wangyaoqianw-max/STM32F4_Bootloader_OTA_Test_Project/blob/main/03_Firmware/Application/OTA_APP/01_APP/system/app_startup.c)
- [STM32F4 Bootloader 项目需求](https://github.com/wangyaoqianw-max/STM32F4_Bootloader_OTA_Test_Project/blob/main/00_Project/01_Requirements/%E9%A1%B9%E7%9B%AE%E9%9C%80%E6%B1%82V1.md)
- [stm32f4_DMA_UART_ring_RTOS](https://github.com/wangyaoqianw-max/stm32f4_DMA_UART_ring_RTOS)
- [DMA UART Ring RTOS Roadmap](https://github.com/wangyaoqianw-max/stm32f4_DMA_UART_ring_RTOS/blob/main/RTT_elog_DMA_UART_ring_project/00_Doc/04_Agent/development_roadmap.md)
- [ILI9341 数据手册/模块资料]：待由实际屏幕模块资料确认
- [XPT2046 数据手册]：按原理图器件标注确认
- [W25Q128 数据手册]：按实际采购型号和 JEDEC ID 确认
