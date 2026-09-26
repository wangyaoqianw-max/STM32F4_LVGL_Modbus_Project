# S01 Implementation Plan

## Metadata

- Stage: `S01 Application 工程初始化与诊断基础`
- Status: `READY_FOR_IMPLEMENTATION`
- Design Commit: 本阶段设计文档与计划同批建立
- Baseline Commit: `fd197f695f0742627470cb54068aadc72a238477`
- Owner: `Project Owner`

## Scope and constraints

实施目标是把当前 CubeMX/Keil 母工程整理成可持续开发的 Application 基线，并接入最小五层框架和完整诊断基础。

强制约束：

- 不修改 Embedded Engineering Library 中的源仓库内容。
- 从 Library 迁入的 `platform_types.h` 必须保持原文件内容，不得修改。
- 不提前实现 S02 以后业务功能。
- 不移动 CubeMX 生成的 `Core/`、`Drivers/`、`Middlewares/`。
- 第三方源码保持上游目录、版权和 License。
- 对 CubeMX 生成文件的必要修改优先放在 USER CODE 区；不能持久化的改动必须记录恢复方式。
- 测试代码和 Fault Trigger 可以临时加入，完成受控验证后恢复默认关闭状态。

## Tasks

### Task 1: 冻结 S01 输入基线

- Inputs:
  - 当前 `.ioc`
  - `.uvprojx`
  - S01 Design
  - 三份 Firmware 参考文档
  - Embedded Engineering Library `8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0`
- Files:
  - 只读检查为主
- Steps:
  1. 记录当前 Commit、Branch、CubeMX/Keil/FW_F4 版本。
  2. 核对 `.ioc`、`.uvprojx`、启动文件、FreeRTOSConfig 和当前 Handler。
  3. 记录当前 Clean Rebuild 基线；若基线失败，先记录真实原因。
  4. 保存当前关键文件哈希或 Git Diff 作为后续 CubeMX Regenerate 对照。
- Verification:
  - 基线身份和初始 Build 状态可追溯。
- Output:
  - S01 verification 中的 Baseline 记录。

### Task 2: 落地 Keil 工程与构建输出规范

- Inputs:
  - `03_Firmware/00_Doc/Keil工程与构建输出规范.md`
- Files:
  - `03_Firmware/Application/STM32F407_APP/MDK-ARM/STM32F407_APP.uvprojx`
  - 根 `.gitignore`（仅在确有缺项时）
- Current finding:
  - 当前 `OutputDirectory = STM32F407_APP\`
  - 当前 `ListingPath` 为空
  - 根 `.gitignore` 已经按目录忽略 `Objects/` 和 `Listings/`
- Steps:
  1. 将 Target Output Directory 改为 `Objects\`。
  2. 将 Listing Directory 改为 `Listings\`。
  3. 保持 HEX 输出。
  4. 如 S01 决定生成 BIN，则通过稳定的 Keil/fromelf Post-build 将 BIN 生成到 `Objects/`，不要把普通 Build Artifact 写入 `06_Output/`。
  5. Clean + Rebuild。
  6. 检查生成物实际落点和 `git status --short`。
  7. 对 `.uvprojx` 中 `CLOCK(25000000)` 仅记录其含义/来源；不得把它当成 MCU 运行时 SYSCLK 事实，除非确认需要修改。
- Verification:
  - Objects/Listings 目录符合规范；
  - Rebuild 完成；
  - Git 不出现普通构建输出。
- Output:
  - 稳定 Keil 构建基线。

### Task 3: 建立五层架构最小框架

- Inputs:
  - Library `original/基于五层架构/`
- Files:
  - `00_Config/`
  - `01_APP/`
  - `02_Service/`
  - `03_Platform/`
  - `04_Impl/`
  - `05_Vendors/`
- Steps:
  1. 创建五层目录和必要 README/占位文件。
  2. 迁入 `03_Platform/platform_common/`。
  3. 迁入 `03_Platform/platform_os/`。
  4. 迁入 `04_Impl/impl_os/freertos/`。
  5. 确认 `platform_types.h` 与 Library Blob `a2d23e8575f31494e55548bde62c15e7407055b9` 内容一致。
  6. 将迁入源码加入 Keil 对应 Group 和 Include Path。
  7. 不在本任务迁入 UART Service、Ring Buffer、W25Q、DHT20、LCD 等后续功能。
- Verification:
  - 基础框架可编译；
  - Platform API 不向 App/Service 泄漏 HAL/RTOS Handle；
  - `platform_types.h` 无改动。
- Output:
  - 五层架构基础骨架。

### Task 4: 接入 SEGGER RTT

- Inputs:
  - Library `third_party/SEGGER_RTT/v7.92/`
  - `RTT_CmBacktrace_AI移植指南.md`
- Files:
  - `05_Vendors/SEGGER_RTT/`
  - Keil Group / Include Path
- Steps:
  1. 原样迁入所需 RTT 源文件和配置。
  2. 只把实际需要编译的源文件加入 Keil。
  3. 选择当前项目的 RTT Buffer 配置，不机械复制 F411/F407 实验值。
  4. 在不依赖 EasyLogger/CmBacktrace 的情况下先验证 RTT 通道。
- Verification:
  - J-Link RTT 可以读取确定的启动测试文本。
- Output:
  - 独立 RTT transport 可用。

### Task 5: 接入 CmBacktrace 与 Fault 诊断链

- Inputs:
  - Library `third_party/CmBacktrace/v1.5.0/`
  - Library `original/diagnostics_solution/`
  - F407 对照实验迁移指南
- Files:
  - `05_Vendors/CmBacktrace/`
  - `00_Config/diagnostics/`
  - `04_Impl/diagnostics/`
  - `stm32f4xx_it.c` / Fault 汇编 / Keil 工程接线
  - FreeRTOS additions 相关文件（若目标版本支持）
- Steps:
  1. 迁入 CmBacktrace 第三方本体。
  2. 迁入 `cmb_user_cfg.h`、CmBacktrace Port、Fault Adapter。
  3. 设置本项目 Firmware/Hardware/Version 字符串。
  4. 将 CmBacktrace 输出直接接 RTT。
  5. 全局检索 HardFault/MemManage/BusFault/UsageFault 定义。
  6. 确定唯一活动 Handler Owner；不得同时编入 Vendor 示例 Fault 汇编和项目 Fault 汇编。
  7. 优先使用 F407 当前 FreeRTOS additions 扩展方式获取任务上下文；除非证据表明不可行，不修改 `tasks.c`。
  8. 初始化放在 HAL/基础初始化完成后、RTOS Scheduler 启动前。
- Verification:
  - Keil 无重复 Handler；
  - CmBacktrace 初始化和打印可通过 RTT 观察；
  - Fault test 默认关闭。
- Output:
  - 最小依赖 Fault Diagnostics。

### Task 6: 接入 EasyLogger + Service Log

- Inputs:
  - Library `third_party/EasyLogger/v2.2.99/`
  - `adapted/easylogger_rtt_cmsisrtos_port/`
  - `original/diagnostics_solution/service_log/`
  - `original/diagnostics_solution/platform_log/`
- Files:
  - `05_Vendors/EasyLogger/`
  - `00_Config/diagnostics/`
  - `02_Service/log/`
  - `03_Platform/platform_log/`
  - `04_Impl/diagnostics/easylogger/`
- Steps:
  1. 原样迁入 EasyLogger 第三方本体。
  2. 迁入 RTT + CMSIS-RTOS2 Port。
  3. 清理仅由来源项目遗留、在当前项目不需要的 include；只做当前工程实际需要的最小适配。
  4. 接入 Platform Log 与 Service Log。
  5. 初始化顺序按 Library Diagnostics 合同确定。
  6. 输出 INFO/WARN/ERROR 基础测试日志。
- Verification:
  - 正常日志经 Service Log → Platform Log → EasyLogger → RTT 可读；
  - RTOS 启动前后日志行为符合 Port 合同；
  - 不把 Fault 输出依赖到 EasyLogger。
- Output:
  - 正常运行日志链完成。

### Task 7: CubeMX Regenerate 回归

- Inputs:
  - 集成完成后的可构建工程
  - Generate 前 Git Diff/Commit
- Files to inspect:
  - `main.c`
  - `stm32f4xx_it.c`
  - `FreeRTOSConfig.h`
  - FreeRTOS additions / kernel hook
  - `.uvprojx`
  - Diagnostics 初始化锚点
- Steps:
  1. 保存 Generate 前状态。
  2. 对当前工程执行一次正常 CubeMX Generate。
  3. 比对前后差异。
  4. 判断哪些改动位于 USER CODE 可保留区，哪些会被生成器覆盖。
  5. 对被覆盖部分形成明确恢复步骤、检查脚本或文档规则；不为了自动化而强行修改 CubeMX/Vendor 源。
  6. 再次 Clean Rebuild。
- Verification:
  - Generate 后工程可恢复到完整诊断状态；
  - Build 再次通过；
  - 形成可重复的 regeneration checklist。
- Output:
  - CubeMX 再生成稳定性结论。

### Task 8: S01 集成验证

- Inputs:
  - 完成 Task 1~7 的工程
- Files:
  - `04_Test/Reports/Stages/S01/verification.md`
- Steps:
  1. Clean Rebuild。
  2. 记录 Error/Warning、AXF/HEX/BIN/MAP 等实际输出。
  3. J-Link 下载。
  4. RTT 正常启动日志验证。
  5. EasyLogger 等级日志验证。
  6. 经受控测试宏启用一种已知 Fault。
  7. 检查 Fault 类型、关键寄存器/PC、CmBacktrace 输出与 GDB/Map/AXF 是否可对应。
  8. 关闭 Fault Test，重新构建/下载正常镜像。
  9. 检查最终 `git status`。
- Verification:
  - 每项明确 PASS / FAIL / PENDING；
  - 无硬件条件时硬件项保持 PENDING。
- Output:
  - S01 verification evidence。

### Task 9: 文档、交接和状态收尾

- Inputs:
  - 实际代码改动
  - verification.md
- Files:
  - `handoff.md`
  - `review.md`（进入 Review 时）
  - `current_status.md`
  - `PROJECT_CONTEXT.md`
  - 必要的 Firmware README
- Steps:
  1. 记录实际迁入资产和来源 Commit。
  2. 记录与 Library 不同的项目适配。
  3. 记录未验证项和下一阶段依赖。
  4. 状态按工作流推进到 READY_FOR_VERIFICATION / READY_FOR_REVIEW。
- Verification:
  - 文档、代码、测试状态一致。
- Output:
  - 可供 Review 和 S02 接手的正式上下文。

## Verification summary

- Code verification: `NOT_RUN`
- Hardware verification: `NOT_RUN`
- Evidence location: `04_Test/Reports/Stages/S01/verification.md`
