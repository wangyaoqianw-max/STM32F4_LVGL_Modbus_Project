# RTT + CmBacktrace AI 移植指南

## 目的

指导 AI 将 RTT 与 CmBacktrace 接入现有 STM32CubeMX + HAL + FreeRTOS + Keil 工程。先识别工程差异，再按语义制定改动和验证计划；不要把 F407/F411 的路径、行号或单一实现照搬到其他工程。

本指南以当前两组配对实验为依据。它提供有边界的迁移流程，不是面向所有 STM32 的无条件补丁。

## 适用范围和证据等级

已对照工程：

- STM32F407VET6：`STM32F407/STM32F407`（Clean）与 `STM32F407_CmBacktrace/STM32F407`（移植）。
- STM32F411CEU6：`STM32F411/STM32F411CEU6`（Clean）与 `STM32F411_CmBacktrace/STM32F411CEU6`（移植）。
- 两组实验环境：CubeMX 6.8.1、STM32Cube FW_F4 V1.27.1、FreeRTOS、Keil MDK。

把结论分为三类：

1. **已观察到的共同语义**：两组配对差异都有证据支持。
2. **项目适配项**：语义相同，但 MCU、FreeRTOS 版本、内核钩子或 Keil 工程结构不同，需要重新识别。
3. **未验证项**：没有实测证据时，不写成已验证行为，例如 CubeMX 再生成后的持久性。

当前工程证据见 [跨 MCU 规则](跨MCU规则.md)、[F407 差异](F407差异.md)、[F411 差异](F411差异.md) 和 [CubeMX 生成边界](CubeMX生成边界.md)。

## 执行规则

- 先读取目标工程的指导文件、README、`.ioc`、入口源码、FreeRTOS 配置和 Keil 工程文件，再规划改动。
- 有 Clean 配对工程时，把它当只读基线；不要覆盖 Clean 工程或用它生成副本。
- 按实际 MCU、FreeRTOS 版本、现有异常入口和 Keil Target 识别锚点；使用语义检查，不使用固定行号。
- 区分事实、推断和未知项。静态差异不能证明 CubeMX Generate 会覆盖或保留某项修改。
- 不因诊断移植顺手改变任务栈、内核最小栈或堆。先确认是哪一种栈/堆参数，以及是否由 `.ioc` 管理。
- 用户未要求时，不运行构建、测试、烧录、调试或 CubeMX Generate；不执行会覆盖目标状态的操作。
- 改动前列出目标文件、拟修改内容和可验证结果；完成后给出文件级差异、构建/硬件验证状态和未解决项。

## 移植流程

### 1. 盘点和冻结基线

确认并记录：

- MCU 完整型号、启动文件、芯片宏、存储布局。
- CubeMX、HAL、FreeRTOS、RTT、CmBacktrace、Keil 和编译器版本。
- `.ioc` 哈希、FreeRTOS 任务创建代码、`FreeRTOSConfig.h` 和 Keil Target。
- Clean 工程当前构建状态；如果基线本身有问题，先记录，不归因到移植。
- 目标工程现有日志接口、异常 Handler 定义和 FreeRTOS 查询钩子。

对比 Clean 和移植工程时，排除 `Listings`、`Objects`、临时目录和普通构建产物；保留对 Keil 工程文件实际引用的检查。

### 2. 接入 RTT

- 识别 RTT 版本和所需源码/配置；只加入需要编译的文件，不以整个 Vendor 目录存在作为“已接入”的证据。
- 在 Keil 工程中登记源文件、头文件路径和必要宏，并确认目标文件只编译一次。
- 根据输出量和目标内存决定缓冲区大小；F407 当前配置为 1024 B，F411 为 4096 B，这两个值不是通用常量。
- 通过实际 RTT 通道读取启动文本，确认 RTT 初始化、缓冲区和调试工具连接都有效。

### 3. 接入 CmBacktrace 打印和配置

- 识别库配置入口，按目标填写芯片/固件标识和输出配置。
- 将 CmBacktrace 打印接口接到 RTT。当前工程通过 `vsnprintf` 格式化后写 RTT 通道 0；移植到其他库版本时仍需核对 API 和格式化要求。
- 检查 `cmb_println` 等输出宏确实指向有效实现，不能停留在空宏或注释示例。
- 将 CmBacktrace 和项目诊断适配层加入 Keil 编译组及 include 路径。

### 4. 统一 Fault Handler 所有权

- 查清目标工程中 HardFault、MemManage、BusFault、UsageFault 的所有定义，包括 C、汇编、弱符号和启动文件入口。
- 选择一套活动实现。当前两份移植工程使用项目自己的 `diagnostics/cmbacktrace_fault_handlers.S`；Vendor 里的 Keil `cmb_fault.S` 没有同时编入。
- 如果汇编入口接管 Handler，禁用 CubeMX 生成的重复 C 处理器，并确认入口把 EXC_RETURN 和异常栈指针传给正确的 C 分发函数。
- 用源码和最终构建输入检查每个 Handler 只有一个有效归属；不要只搜索声明，也不要按行号删除函数。

### 5. 适配 FreeRTOS 任务上下文

CmBacktrace 要解析线程栈时，需要准确取得当前任务名、栈起点/边界等信息。按目标 FreeRTOS 内核选择接口：

- 检查是否提供 `configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H` 及 additions 头文件钩子；存在且适用时优先评估项目侧扩展。
- 没有合适扩展点时，才考虑对内核 `tasks.c` 做最小补丁，并记录 FreeRTOS 版本、改动位置和回归检查。
- 根据库要求确认是否需要 `configRECORD_STACK_HIGH_ADDRESS`。
- 不按 MCU 名称硬编码实现方式：当前 F407 使用 additions 头文件，F411 移植版本改了 `tasks.c`；这是内核/工程选择差异。

把以下数值分开记录，不能统称“栈大小”：

- CMSIS-RTOS 任务栈，例如 `defaultTask`。
- FreeRTOS `configMINIMAL_STACK_SIZE`。
- FreeRTOS `configTOTAL_HEAP_SIZE`。
- Cortex-M 主栈及 CubeMX ProjectManager 的 Stack/Heap。

当前两组配对中任务栈、内核最小栈和 FreeRTOS 堆没有因诊断移植而增加。需要改变、且 `.ioc` 能表达的配置，应先在 `.ioc` 中设置并核对生成代码。

### 6. 安排初始化

- 在主初始化流程中找到 HAL/板级初始化与调度器启动边界。
- 在硬件基础初始化后、FreeRTOS 调度器启动前调用 RTT、CmBacktrace 和 Fault 上下文初始化。
- 优先放在 CubeMX 明确保留的用户代码区；记录初始化顺序依赖。
- 不要把测试用 Fault 触发器作为正常运行必需代码。测试宏默认关闭，受控测试结束后恢复正常镜像。

### 7. 检查 Keil 工程接线

核对 `.uvprojx` 中：

- RTT、CmBacktrace、诊断 C/汇编文件已加入正确 Target 和 Group。
- include 路径、预处理宏和必要的编译选项与源代码配置一致。
- 不存在重复 Handler、重复 RTT 实现或只在磁盘上存在但未参与编译的文件。
- 目标器件、Flash 算法、调试接口和下载设置与目标板匹配；不要把一个 Target 的设置套到另一 MCU。

### 8. 分层验证

按顺序记录每一步，不用单个“Build/Download 成功”替代功能验收：

1. Clean 基线构建结果。
2. 移植工程构建结果、错误/警告和映像大小。
3. RTT 启动信息与实际通道。
4. Handler 唯一性、诊断源文件和 FreeRTOS 上下文接口参与构建。
5. 经用户授权后，在可控条件下触发已知 Fault，核对 EXC_RETURN、异常栈、任务信息、寄存器和回溯帧。
6. 测试后恢复正常宏/镜像，并确认正常启动。

当前 F411 有受控 Fault 回溯通过记录；F407 的 Fault 回溯尚未验证。F407 曾有 RTT 启动输出记录，但最近一次恢复后的复读未捕获文本，须保留该差异。

### 9. 单独处理 CubeMX 再生成

静态配对没有测试 Generate。若用户要求验证再生成：

- 先保存目标工程差异、当前可恢复状态和版本信息。
- 明确只对用户指定的移植工程执行；Clean 基线保持只读。
- Generate 后检查 `main.c`、`stm32f4xx_it.c`、`FreeRTOSConfig.h`、FreeRTOS 扩展/内核文件和 `.uvprojx`。
- 重新构建并按需求重复硬件验证。
- 只有生成前后实测过的结果才能写成“可持久化”或“会被覆盖”；否则标为未验证。

## 共同流程与需要适配的部分

| 主题 | 可以复用的语义 | 每个工程必须重新检查 |
|---|---|---|
| RTT | 配置后提供可读输出通道 | 源码版本、配置头、缓冲大小、Keil 文件组 |
| CmBacktrace 输出 | 统一接到 RTT 输出接口 | 格式化 API、固件/芯片标识、编译宏 |
| 异常处理 | 四个 Fault Handler 只有一套活动实现 | 启动文件、HAL Handler、汇编入口、EXC_RETURN/SP 约定 |
| FreeRTOS | 提供当前任务名和栈上下文 | 内核版本、扩展钩子、栈单位与查询实现 |
| 初始化 | 在调度器启动前完成诊断初始化 | USER CODE 锚点、依赖顺序、是否可再生成保留 |
| 构建工程 | 所需源文件、宏和 include 路径都进入目标 | Keil Target、工程格式、器件路径和编译器设置 |
| 栈/堆 | 每种资源单独识别，不因“诊断”默认加大 | `.ioc` 参数来源、生成值和实际使用单位 |

EasyLogger 在两份移植工程中虽有 Vendor 文件，但未进入 Keil 构建或应用调用链；本轮 RTT + CmBacktrace 不要求加入 elog。若目标另有日志等级、标签过滤或异步日志需求，再单独评估。

## 自动化边界

目前适合先自动化**只读识别和验收**：

- 工程/MCU/`.ioc`/版本和文件清单。
- Handler 定义与 Keil 编译输入的交叉检查。
- RTT/CmBacktrace 源文件、include、宏和初始化锚点检查。
- 任务栈、内核最小栈、堆及 ProjectManager 栈/堆配置分别提取。
- Clean 与移植工程差异分类和证据报告。

暂不把以下动作做成无条件自动修改：删除 Handler、改 FreeRTOS 内核、覆盖配置文件、运行 CubeMX Generate、烧录硬件。应先检测目标形态、输出变更计划，并受当前用户授权约束。补丁自动化需在更多内核版本和再生成实测后再定。

## AI 输出格式

每次迁移至少输出：

1. 目标工程身份、适用范围和基线状态。
2. 发现的 RTT、CmBacktrace、Fault Handler、FreeRTOS、Keil 接入点。
3. 计划改动表：文件/语义锚点、理由、动作、验证方法、是否可由 `.ioc` 表达。
4. 共性、项目特有、可选项和未确认项。
5. 构建、RTT、Fault 和再生成验证结果；未执行的项目明确标记“未验证”。
6. 是否保留 Clean 工程不变，以及测试结束后的目标恢复状态。

不要仅输出“移植成功”；要给出实际构建和硬件证据，并注明本次没有执行的检查。
