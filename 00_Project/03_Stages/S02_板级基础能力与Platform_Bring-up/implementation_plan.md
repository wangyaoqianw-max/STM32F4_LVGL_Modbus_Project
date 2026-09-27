# S02 Implementation Plan

## Metadata

- Stage: `S02 板级基础能力 / Platform Bring-up`
- Status: `CLOSED`
- Design Commit: 本阶段设计文档与计划同批建立
- Baseline Commit: `0ffe747eeb0a1e7e2c6a088e2b402bbc57965f44`
- Reuse Baseline: `wangyaoqianw-max/Embedded_Engineering_Library@8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0`
- Owner: `Project Owner`

## Scope and constraints

实施目标是把已有 MCU Platform / STM32F4 Impl 基础资产接入当前 STM32F407VET6 Application 工程，完成底层资源 Binding 和最小必要验证。

强制约束：

- 不实现完整设备初始化链。
- 不提前开发 S03+ 的 Driver / Service。
- 不迁入 Watchdog。
- 不新增通用 `platform_dma` 或 `platform_fmc`。
- `platform_types.h` 保持冻结 Library Blob，不因代码风格修改。
- Service / App 不得直接访问 HAL Handle、GPIO Port/Pin 或 DMA Handle。
- 所有 F411 来源假设必须重新核对，不得直接当作 F407 硬件事实。
- 板测只覆盖能够证明当前底层 Binding 的关键链路，避免重复测试。

## Tasks

### Task 1: 冻结 S02 输入与复用范围

**Inputs:**

- 当前 STM32F407 Application 工程；
- S02 Design；
- Pinout/CubeMX 外设与引脚基线；
- Embedded Engineering Library 冻结 Commit。

**Steps:**

1. 记录施工开始时项目 Commit、Branch 和 Library Commit。
2. 对照 Library 列出 GPIO、Delay、Software I2C、SPI、UART、IRQ、Reset 的 Platform / Impl 文件。
3. 搜索来源代码中的 F411、具体 Handle、GPIO、时钟和板级宏假设。
4. 确认 Watchdog 不进入本阶段。
5. 确认 `platform_types.h` 仍与冻结 Blob 一致。

**Verification:** 输出迁入清单与需要适配的板级差异，不修改 Library 源仓库。

### Task 2: 迁入 MCU Platform / STM32F4 Impl 基础资产

**Target areas:**

- `03_Firmware/Application/STM32F407_APP/03_Platform/`
- `03_Firmware/Application/STM32F407_APP/04_Impl/`
- Keil Groups / Include Paths

**Steps:**

1. 迁入 GPIO、Delay、Software I2C、SPI、UART、IRQ、Reset 对应 Platform 接口。
2. 迁入 STM32F4 Backend。
3. 保持 Library API 结构，只有当前 F407 真实差异才做最小适配。
4. 将源文件加入正确 Keil Group / Include Path。
5. 检查依赖方向，不允许 Platform 反向依赖 Service / App。
6. 不迁入当前阶段不需要的 UART Service、RingBuffer、W25Q、AT24、DHT20 等资产。

**Verification:** 工程可编译到链接阶段；无重复符号和明显跨层依赖。

### Task 3: 完成 F407 Board / HAL Binding

**Binding targets:**

- SPI1 / `hspi1`；
- USART1 / `huart1`；
- USART2 / `huart2`；
- USART3 / `huart3`；
- PB6/PB7 Software I2C；
- 本阶段测试需要的 GPIO；
- 必要 IRQ / Callback 接线路径。

**Steps:**

1. 检查 Library 现有 Handle 注入/注册接口。
2. 优先使用已有接口绑定当前 CubeMX HAL Handle。
3. 若需要项目级薄型 Adapter，放在 `04_Impl/impl_board`，只表达 MCU/Board Binding。
4. 不在 Binding 层加入 W25Q128、OTA、Bluetooth、Modbus、DHT20 等设备/业务语义。
5. 不建立依赖未来全部设备存在的总初始化函数。
6. 核对 HAL 初始化发生在 Binding 可使用资源之前；当前无法冻结的完整 Driver/Service 初始化顺序保持后续阶段决定。

**Verification:** Platform 调用可落到正确 F407 Impl/HAL 资源，上层不需要知道具体 Handle。

### Task 4: Build / Static Review

**Steps:**

1. Clean Rebuild。
2. 记录编译器、Error/Warning 和镜像结果。
3. 全局搜索 App/Service 对 `hspi*`、`huart*`、GPIO Port/Pin、DMA Handle 的直接依赖。
4. 搜索残留 F411 专用符号或错误板级配置。
5. 检查 UART DMA Stream/Channel、USART IRQ Priority 与当前 CubeMX 基线一致。
6. 检查 FMC 仍由 CubeMX/HAL 拥有，没有新增 `platform_fmc`。
7. 检查没有新增 `platform_dma`。

**Verification:** Clean Rebuild PASS；无 S02 引入的新跨层依赖。

### Task 5: GPIO 最小 Smoke Test

**Steps:**

1. 从已确认 Pinout/原理图中选一个可观察 GPIO。
2. 通过 Platform GPIO API 驱动。
3. 用板上可观察现象或逻辑分析仪确认物理引脚状态变化。
4. 测试代码保持最小，并在验证完成后移除或默认关闭。

**Verification:** `Platform GPIO → STM32F4 Impl → HAL → Physical Pin` 链路成立。

### Task 6: Software I2C + AT24C02 ACK Probe

**Steps:**

1. 使用 PB6/PB7 Software I2C Binding。
2. 通过通用 I2C 起始/地址/停止流程探测板载 AT24C02 地址 ACK。
3. 不引入完整 AT24C02 Driver，不执行 EEPROM 写入。
4. 优先使用逻辑分析仪观察 SCL/SDA、START、地址、ACK、STOP。
5. 检查空闲总线高电平；若异常，先验证外部上拉事实。
6. 不以开启 MCU 内部上拉作为未经审查的永久修复。

**Verification:** 地址 ACK 与波形证据成立；若硬件上拉条件阻塞则明确记录 `BLOCKED/PENDING`。

### Task 7: UART 最小 Blocking TX/RX Smoke

**Steps:**

1. 根据当前实际接线条件，从 USART1/2/3 中选择最方便的一路。
2. 使用 Platform UART API 完成最小 Blocking TX。
3. 完成最小 Blocking RX 或可重复 Echo/Host 回读测试。
4. 只验证当前选择通道，不为了覆盖率重复测试三路。
5. 不启用 DMA Circular RX、IDLE、RingBuffer、Task Notification 或 UART Service。

**Verification:** 至少一路 `Platform UART → STM32F4 Impl → HAL UART → Physical UART` 链路有真实板级结果。

### Task 8: SPI / IRQ / DMA / FMC 边界核对

**SPI:**

- 确认 `hspi1` Binding 与 Platform SPI API 可编译、可初始化。
- 不做重复 Loopback。
- W25Q128 JEDEC/Read 留 S03。

**IRQ / DMA:**

- 核对现有 CubeMX mapping 和 FreeRTOS IRQ priority。
- 核对 Library UART Callback 路由所需接线是否与当前 HAL Callback 机制兼容。
- 不建立正式 DMA Runtime。
- 不新增 `platform_dma`。

**FMC:**

- 检查 CubeMX/HAL 配置仍存在且没有被 S02 改坏。
- 不创建 `platform_fmc`。
- LCD/FMC timing 与实机显示留 S06。

**Verification:** 所有未执行真实设备测试的项目在 verification 中明确写为 `DEFERRED`，不能写成 PASS。

### Task 9: 阶段验证、Review 与交接

**Files:**

- Create: `04_Test/Reports/Stages/S02/verification.md`
- Create: `00_Project/03_Stages/S02_板级基础能力与Platform_Bring-up/handoff.md`
- Create: `00_Project/03_Stages/S02_板级基础能力与Platform_Bring-up/review.md`
- Modify: `PROJECT_CONTEXT.md`
- Modify: `00_Project/05_Status/current_status.md`
- Modify: `README.md`

**Steps:**

1. 汇总 Build、Static Check、GPIO、Software I2C、UART 的实际证据。
2. 明确 SPI/W25Q128、UART DMA Runtime、FMC/LCD 等 Deferred 项。
3. 独立 Review 架构边界、API、构建、测试和文档一致性。
4. Review 发现必须回到对应任务修正，不通过降低验收定义关闭阶段。
5. Review PASS 后将 S02 标记 CLOSED，并把下一工作项切到 S03。

**Verification:** Stage docs、verification、项目状态和实际仓库内容一致。

### Task 10: 提交与推送

1. 检查最终 diff，只包含 S02 计划内修改。
2. 执行空白/格式检查。
3. 提交 S02 实现。
4. 记录 Implementation Commit 到 handoff/status/context。
5. 推送当前分支并确认远端同步。

## Verification strategy

按项目测试原则：

```text
Static Check
    ↓
Clean Rebuild
    ↓
最小自动/板级 Smoke
    ├─ GPIO
    ├─ Software I2C ACK
    └─ UART Blocking
    ↓
Independent Review
```

不在 S02 对 SPI/W25Q128、UART DMA、FMC/LCD 做重复板测。

## Completion definition

S02 只有在以下条件全部满足后才可关闭：

- 选定 Platform MCU / STM32F4 Impl 资产迁入并适配完成；
- F407 Board/HAL Binding 清晰；
- Build PASS；
- GPIO Smoke 有真实证据；
- Software I2C ACK 有真实结果或有明确硬件阻塞证据；
- UART Blocking Smoke 有真实结果；
- SPI/DMA/FMC Deferred 边界记录准确；
- 无 Watchdog、完整设备 Driver、UART Service 等范围膨胀；
- Review PASS；
- Handoff / Verification / Status 同步。

## Execution gate

本计划已经由 Project Owner 在设计讨论中批准，状态为 `READY_FOR_IMPLEMENTATION`。正式施工时先重新读取当前仓库 HEAD；若 HEAD 已偏离本计划 Baseline，先检查差异是否影响 S02，再开始修改。
