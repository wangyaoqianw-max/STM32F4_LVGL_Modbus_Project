# S01 Verification

## Metadata

- Stage: `S01 Application 工程初始化与诊断基础`
- Status: `READY_FOR_REVIEW`
- Design Baseline: `fd197f695f0742627470cb54068aadc72a238477`
- Source Commit: `0a8d61383564d62168431676c86c799c4abd240b`
- Verification Commit: 随 S01 完成交付提交；以 `main` Git 历史为准
- Date: `2026-09-27`

## Environment

- MCU / Keil Target: `STM32F407VET6` / `STM32F407_APP`
- CubeMX: `6.8.1`
- STM32Cube FW_F4: `V1.27.1`
- Keil MDK: `5.38.0.0`
- Compiler: `ARM Compiler V5.06 Update 7 (build 960)`
- J-Link Commander: `V7.92`；首次连接返回固件标识 `J-Link V9 compiled Dec 8 2023 20:16:22`
- Board: 实物板卡型号未由丝印或原理图单独确认；本工程软件目标为 STM32F407VET6
- Latest successful J-Link observation: `STM32F407VE` / SWD / 1 MHz / VTref `3.301 V`.
- Branch: `main`
- Build tool: `C:\Users\17258\.agents\skills\keil\scripts\keil_build.py`
- J-Link executable: `C:\Program Files\SEGGER\JLink\JLink.exe`

## Baseline build

- Clean Rebuild: `PASS`
- Errors: `0`
- Warnings: `0`
- AXF / HEX: 均生成；AXF 映像大小报告为 19,656 bytes，HEX 文件大小 55,340 bytes
- Initial Keil output path: `03_Firmware/Application/STM32F407_APP/MDK-ARM/STM32F407_APP/`
- Retained build artifacts: `06_Output/S01/PreOutputConfigBuild/MDK-ARM/STM32F407_APP/`
- Build log: `06_Output/S01/BuildLogs/STM32F407_APP-STM32F407_APP-rebuild.log`
- HEX SHA-256: `C124A87060A44C11BA798D06BE6DECAA520DA8669078B91FBF7C838E484B7510`
- Historical note (2026-09-26 baseline): 当时 `.uvprojx` 配置 `OutputDirectory=STM32F407_APP\`、`ListingPath` 为空；首轮生成的 130 个输出文件和启动 Listing 移入 `06_Output/S01/PreOutputConfigBuild/`，HEX 移动前后 SHA-256 一致。项目负责人后来确认曾手动操作 Keil，非规范输出目录随后被重新生成；该次输出不作为本轮构建证据。Task 3 已在 2026-09-27 核验旧目录只含 Keil 构建产物并归档到 `06_Output/S01/LegacyBuildArtifacts/`。
- Git status: 本次没有修改固件源码；本地 Keil 工程/用户设置文件改动保留，详见最终 Git 状态。

## Basic Build / Flash / Run initial attempt (2026-09-26)

- Clean Rebuild: `PASS`，0 Error / 0 Warning。
- Build output: AXF / HEX 已生成，路径与 SHA-256 见 Baseline build。
- J-Link connect: 首次 `PASS`；SWD、1 MHz，目标电压 `VTref=3.283 V`。后续复核 `FAIL`，退出码 1，未再取得固件或目标电压信息。
- Target identification: 首次 `PASS`，J-Link 按 `STM32F407VE` 目标选择；与本项目 `STM32F407VETx` / `STM32F407VET6` 目标相符。未使用 Bootloader 工程的 F411 目标配置。
- Flash: `FAIL`。首次 Flash 调用返回 `cannot_connect_target`，无成功下载或校验标志；原始命令复测输出：`Connecting to J-Link via USB...Out of sync, resynchronizing...`、`FAILED: Cannot connect to J-Link`。没有证据证明 HEX 已写入。
- Reset / Run: `PENDING`。因 Flash 未通过，未执行独立的运行路径确认。
- Evidence command: J-Link wrapper 使用 `info` / `flash`，显式参数 `--device STM32F407VE --interface SWD --speed 1000`；Flash 输入为当前基线 HEX。输出中未记录或展示探针序列号。
- Initial attempt result: `FAIL`（首次尝试时基础闭环未完成；2026-09-27 恢复施工后的结果见下节）。
- Follow-up at pause: 项目负责人指示暂停；后续实际选择 J-Link 重试并通过。CMSIS-DAP 未运行。

## DAPLink 路径核对与暂停记录

- 项目负责人报告 DAPLink 可正常使用；这是用户提供的信息，本轮没有独立连接日志，不能记作工具验证 `PASS`。
- 复核发现 Keil 当前原始选项的活动调试 DLL 为 `Segger\JL2CM3.dll`。此前被误认为 DAP 的 Keil Flash 调用日志实际出现 J-Link 信息，并报告 `Failed to configure AP`、读取 AIRCR 的 DAP 错误及 `Flash Download failed`；该次按 J-Link Flash `FAIL` 记录，不计作 CMSIS-DAP 尝试。
- 为避免覆盖本机 Keil 用户选项，曾准备临时工程副本，将调试 DLL 设为 `BIN\CMSIS_AGDI.dll`，并核对器件 `STM32F407VETx`、Target `STM32F407_APP`。项目负责人要求停止时，尚未启动这份临时工程的连接或下载操作；临时工程和选项文件已移除，正式 `.uvprojx` / `.uvoptx` 未因本次临时切换而改写。
- CMSIS-DAP Connect / Flash / Reset / Run：`NOT_RUN`。截至 2026-09-26 暂停时，Task 2 Flash 为 `FAIL`、Reset / Run 为 `PENDING`；2026-09-27 恢复后的 J-Link 结果见下一节。
- 当时暂停在 Task 2；本轮恢复后按最新用户指示使用 J-Link 完成了 Task 2，再进入 Task 3。
- 暂停收尾时检测并终止后台 `JLinkGUIServer` 与无窗口标题的 `UV4` 进程；复查未发现 J-Link、Keil、OpenOCD 或 probe-rs 相关进程残留。

## 恢复施工后的 Task 2 验证（2026-09-27）

- 项目负责人拔插 J-Link 后重试；本次 Connect `PASS`，选择器为 `STM32F407VE`，接口 SWD，速度 1 MHz，VTref `3.298 V`。不据此推断此前失败的根因。
- Flash `PASS`：输入为 `06_Output/S01/PreOutputConfigBuild/MDK-ARM/STM32F407_APP/STM32F407_APP.hex`，工具报告校验通过（`verified=true`）。HEX SHA-256：`C124A87060A44C11BA798D06BE6DECAA520DA8669078B91FBF7C838E484B7510`。
- Reset `PASS`。
- Run `PASS`：通过 map 符号在 `osDelay` 处设置运行断点并命中；该调用来自 `StartDefaultTask`，随后继续运行、Halt 并再次恢复运行，确认进入 FreeRTOS Task 路径。此项不验证外设功能。
- 当前 Task 2 最小 Build → Flash → Reset / Run 闭环：`PASS`。之前 J-Link 失败记录保留为历史结果。
- CMSIS-DAP Connect / Flash / Reset / Run：`NOT_RUN`。本次未通过用户报告替代实际连接验证。
- 原始 J-Link 输出未持久保存；脱敏后的结果摘要：`06_Output/S01/RuntimeEvidence/Task2-JLink-2026-09-27.json`。未在证据文件记录探针序列号。

## Task 3: Keil build output routing (2026-09-27)

- Target `STM32F407_APP` 的 `.uvprojx` 当前设置：`OutputDirectory=.\Objects\`、`ListingPath=.\Listings\`、`CreateHexFile=1`。
- 实际构建还受本地 `.uvoptx` 的 `ListingPath` 影响。最初该字段为空；仅改 `.uvprojx` 后，Clean Rebuild 仍把 MAP 写入 Objects、启动 `.lst` 写入 MDK-ARM 根目录。将 `.uvoptx` 的该字段设为 `.\Listings\` 后复建，输出落点通过文件路径和本次时间戳确认。
- 最终 Clean Rebuild：`PASS`，ARM Compiler V5.06 Update 7 (build 960)，0 Error / 0 Warning。
- AXF / HEX：`03_Firmware/Application/STM32F407_APP/MDK-ARM/Objects/STM32F407_APP.axf`、`.../Objects/STM32F407_APP.hex`。
- MAP / Listing：`03_Firmware/Application/STM32F407_APP/MDK-ARM/Listings/STM32F407_APP.map`、`.../Listings/startup_stm32f407xx.lst`；两者时间戳为本次 Clean Rebuild。
- 最终构建日志：`06_Output/S01/BuildLogs/Task3-UserOptionsListingPath/STM32F407_APP-STM32F407_APP-rebuild.log`。
- 错误落点遗留物：原 `MDK-ARM/STM32F407_APP/` 内 130 个文件和根目录旧 `.lst` 已移至被忽略的 `06_Output/S01/LegacyBuildArtifacts/`；归档前后哈希清单一致。当前 Git 未显示 Objects/Listings 构建物。
- Keil 的 `CLOCK(25000000)` 来自设备数据库 Default CPU clock 调试参数，不应解释为运行时 SYSCLK；未修改时钟。来源：[Keil Device Database Parameters](https://www.keil.com/support/man/docs/uv4/uv4_c_dd_parameters.asp)。
- Task 3 result: `PASS`。

## Architecture / asset integration

- Library repository: `Embedded_Engineering_Library` Commit `8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0`，本地 HEAD 相同且工作区干净。
- Reused assets: 五层资产 V1.3 的 `platform_common/`（11 files）、`platform_os/`（11 files）、`impl_os/freertos/`（10 files），共 32 个文件；逐文件 Git Blob 核对 `PASS`。
- Supporting dependency: 冻结的 `platform_types.h` 直接 `#include "board_types.h"`，因此额外按同一 Commit 原样迁入 `04_Impl/impl_board/README.md` 和 `board_types.h`。此头只提供基础类型别名，不含本项目板级引脚或硬件事实。
- Project-owned documentation: 在 `00_Config/`、`01_APP/`、`02_Service/`、`03_Platform/`、`04_Impl/` 新增中文 README；未改 Library 源码。
- `platform_types.h`: Blob 为 `a2d23e8575f31494e55548bde62c15e7407055b9`，迁入后再次计算相同，`PASS`；未修改、重构或替换。
- Keil: 新增 4 个 Group、30 个文件条目、7 个 Include Path。新增 11 个 C 文件均在 Clean Rebuild 日志出现且生成对应 `.o`；Clean Rebuild `PASS`，0 Error / 0 Warning。
- API boundary: 检查 17 个 Platform 公共头文件，没有 HAL、FreeRTOS 或 CMSIS-RTOS 具体句柄类型；6 个 OS 抽象对象使用 Library 定义的通用 `void *native` opaque handle。FreeRTOS 适配走现有 CMSIS-RTOS2 wrapper，未修改 Kernel 或 `tasks.c`。
- Task 4 时用户预置的 `05_Vendors/` RTT、CmBacktrace、EasyLogger 未纳入架构骨架；Task 5 已核验并接入 RTT，CmBacktrace / EasyLogger 仍按计划分阶段处理。
- 未迁入 `platform_mcu`、Ring Buffer、UART Service、W25Q、DHT20、LCD 或 Diagnostics 资产。
- Task 4 result: `PASS`。

## Task 5: SEGGER RTT (2026-09-27)

- Reused Library `third_party/SEGGER_RTT/v7.92/`；预置 RTT 文件与冻结 Commit `8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0` 的 Blob 一致，补入同版本 README。目录为 `05_Vendors/SEGGER_RTT/`，第三方源码与 License 未改。
- `SEGGER_RTT_Conf.h` 实际配置：UP Buffer 1024 B、DOWN Buffer 16 B、printf Buffer 64 B、`SEGGER_RTT_MODE_NO_BLOCK_SKIP`。本任务只将 `SEGGER_RTT.c` 加入 Keil；`SEGGER_RTT_printf.c` 和 ARMASM 文件未加入，启动测试只调用 `SEGGER_RTT_WriteString`，ARM Compiler V5 不需要该 ARMASM 实现。
- 在 `main.c` USER CODE 区临时发送 `S01 RTT Task5 startup: PASS`，独立于 EasyLogger、CmBacktrace 和 RTOS 日志链。J-Link RTT Channel 0 在 8 秒捕获中匹配文本：`PASS`。
- 临时探针和 include 已移除；`main.c` Git Blob 恢复为 `82b970afaa08f80b70b6dcc535b81fbbe0424eac`，没有测试逻辑留在正常运行路径。
- RTT 启动测试 Clean Rebuild：`PASS`，0 Error / 0 Warning；日志 `06_Output/S01/BuildLogs/Task5-RTT-StartupTest/STM32F407_APP-STM32F407_APP-rebuild.log`。
- 默认恢复 Clean Rebuild：`PASS`，0 Error / 0 Warning，Flash 19,656 bytes、RAM 20,848 bytes；日志 `06_Output/S01/BuildLogs/Task5-RTT-DefaultRebuild/STM32F407_APP-STM32F407_APP-rebuild.log`。
- 本次新生成 `MDK-ARM/Listings/startup_stm32f407xx.lst`，71,791 bytes，写入时间 `2026-09-27 10:40:48 +08:00`；AXF/HEX 在 `MDK-ARM/Objects/`。`.lst` 已通过本次 Clean Rebuild 实际确认落入 Listings。
- 默认 HEX J-Link 烧录与校验：`PASS`，`verified=true`，设备 `STM32F407VE` / SWD / 1 MHz；下载后命令执行 Reset (`r`) 和 Run (`g`)。此项未做恢复镜像后的符号级运行路径确认，完整硬件运行链由 Task 9 再验。
- 脱敏运行记录：`06_Output/S01/RuntimeEvidence/Task5-RTT-2026-09-27.json`。未保存探针序列号。
- Task 5 result: `PASS`。
## Task 6: CmBacktrace and Fault diagnostics (2026-09-27)

- Reused CmBacktrace v1.5.0 and diagnostics adapter assets from Embedded Engineering Library Commit `8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0`; vendor files and License remain in the existing vendor directory.
- `cmbacktrace_port_init()` and `diagnostics_fault_init()` are called after HAL initialization and before RTOS initialization. CmBacktrace output is routed directly to RTT; no EasyLogger dependency is present.
- Fault ownership audit: startup weak fallbacks retained; four C handlers removed from `stm32f4xx_it.c`; project `cmbacktrace_fault_handlers.S` is the only linked strong owner. Vendor `cmb_fault.S` remains present but excluded from Keil. The clean-build MAP placed all four handlers in `cmbacktrace_fault_handlers.o`; no duplicate definition.
- FreeRTOS context uses the existing `configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H` extension and `configRECORD_STACK_HIGH_ADDRESS`; Kernel `tasks.c`, task stack sizes, heap size, and main stack size were not changed.
- Controlled runtime test temporarily enabled `DIAG_FAULT_TEST_ENABLE` and selected an undefined-instruction UsageFault from `StartDefaultTask` after the configured delay. J-Link RTT Channel 0 captured 34 records: `CFSR=0x00010000`, `FAULT PC=0x080002B6`, firmware `STM32F407_APP` / `STM32F407VET6` / `S01-dev`, `Fault on thread defaultTask`, and thread stack information. CmBacktrace/Fault RTT output: `PASS`. Capture: `06_Output/S01/RuntimeEvidence/Task6-FaultTest-RTT-2026-09-27.txt`.
- The temporary trigger and macro change were restored byte-for-byte from pre-test copies; `DIAG_FAULT_TEST_ENABLE` is back to `(0U)`. No trigger remains in the normal execution path.
- Default recovery Clean Rebuild: `PASS`, 0 Error / 0 Warning, Flash 28,884 bytes / RAM 22,608 bytes; log `06_Output/S01/BuildLogs/Task6-DefaultRecovery/STM32F407_APP-STM32F407_APP-rebuild.log`. J-Link Flash: `PASS`, `verified=true`; the script issued Reset/Run. Independent post-recovery `run-to` / register observation: `PENDING` because both tool invocations failed to return; their J-Link processes were stopped and no J-Link process remains. Task 9 will verify the final normal image through EasyLogger RTT and runtime observation.
- C listing output: `.uvprojx` target field `RvctClst=1`. A Clean Rebuild generated 59 C/assembly `.lst` files under `03_Firmware/Application/STM32F407_APP/MDK-ARM/Listings/`; `main.lst`, `stm32f4xx_it.lst`, and `startup_stm32f407xx.lst` were confirmed. Build outputs are ignored by Git.
- An experiment changing `GenerateListings` did not produce C listings and was reverted; the effective C listing switch was `RvctClst`.
- Task 6 diagnostic integration result: `PASS`. At that checkpoint GDB had not yet been run because PATH and the MDK tool directory did not contain GDB; Task 9 later completed the offline GDB / Map / AXF / Listing correlation using the executable from the prior Bootloader project's local configuration.

## Task 7: EasyLogger + Service Log (2026-09-27)

- Reused EasyLogger v2.2.99 and the Library Service Log / Platform Log assets from Commit `8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0`.
- Normal path: Service Log → Platform Log → EasyLogger → RTT. Fault Adapter / CmBacktrace retains its direct RTT path and does not depend on this normal log chain.
- `ELOG_ASYNC_OUTPUT_ENABLE` stays disabled; no extra logger task or stack allocation was introduced.
- DAPLink RTT captured INFO / WARN / ERROR both before scheduler start and from `defaultTask`: `PASS`. Evidence: `06_Output/S01/RuntimeEvidence/Task7-EasyLogger-DAP-run-2026-09-27.txt`.
- Temporary level markers and heartbeat were removed; `freertos.c` was byte-for-byte restored. A fresh default-image run emitted only EasyLogger and Service Log initialization lines, with no test markers.
- Default-restored Clean Rebuild: `PASS`, 0 Error / 0 Warning, Flash 37,108 bytes / RAM 23,936 bytes; log `06_Output/S01/BuildLogs/Task7-Default-Restored/STM32F407_APP-STM32F407_APP-rebuild.log`.
- Task 7 result: `PASS`.

## Task 8: CubeMX regeneration regression (2026-09-27)

- CubeMX CLI loaded `STM32F407_APP.ioc`, returned `project generate OK`, then exited with code 0. The local executable is CubeMX `6.8.1-RC4`; the `.ioc` records `MxCube.Version=6.8.1` and Firmware Package `STM32Cube FW_F4 V1.27.1`.
- Before/after SHA-256 manifests each contain 1,185 files under `STM32F407_APP` (excluding `MDK-ARM/Objects` and `MDK-ARM/Listings`). Changed / added / removed files: `0 / 0 / 0`. Evidence: `06_Output/S01/CubeMX_Generate/before_manifest.csv`, `after_manifest.csv`, `generate-diff-summary.txt`, and `generate-offline.log`.
- Therefore this Generate did not overwrite or alter diagnostics, USER CODE anchors, `.uvprojx`, or CubeMX source files; no recovery procedure was needed. Repeatable checklist: save Git state and a SHA-256 manifest; run one Generate; compare added/removed/changed paths; inspect `main.c`, `stm32f4xx_it.c`, `FreeRTOSConfig.h`, FreeRTOS additions, `.uvprojx`, and diagnostics init anchors; then Clean Rebuild.
- Post-Generate Clean Rebuild: `PASS`, 0 Error / 0 Warning, Flash 37,108 bytes / RAM 23,936 bytes; log `06_Output/S01/BuildLogs/Task9-PostGenerate-Clean/STM32F407_APP-STM32F407_APP-rebuild.log`.
- One CLI Generate was executed in this run; following the user's instruction, no further Generate was run.
- Task 8 result: `PASS` for the installed CubeMX `6.8.1-RC4`, with the `.ioc` / executable version difference recorded above.

## Task 9: S01 integration verification (2026-09-27)

- Final default Clean Rebuild after GDB inspection and test restoration: `PASS`, 0 Error / 0 Warning, Flash 37,108 bytes / RAM 23,936 bytes; log `06_Output/S01/BuildLogs/Task9-Default-Recovery-After-GDB/STM32F407_APP-STM32F407_APP-rebuild.log`.
- Artifacts: `MDK-ARM/Objects/STM32F407_APP.axf` and `.hex`; MAP and 66 C/assembly `.lst` files are in `MDK-ARM/Listings/`, including fresh `freertos.lst` and `startup_stm32f407xx.lst`.
- Final J-Link Connect: `PASS`, selected device `STM32F407VE` / SWD / 1 MHz, VTref `3.301 V`. Final default HEX Flash: `PASS`, `verified=true`; the flash script issued Reset and Run.
- Final J-Link RTT Channel 0 capture: `PASS`, 8 seconds, exit code 0. It returned EasyLogger v2.2.99 and Service Log initialization lines, with no temporary test markers. Summary: `06_Output/S01/RuntimeEvidence/Task9-Completion-2026-09-27.md`; raw Flash / RTT results are in the neighboring JSON / JSONL files.
- Historical intermittent J-Link connection failures remain unexplained; the latest complete Connect / Flash / Reset / Run / RTT chain passed.
- DAPLink/probe-rs also ran the final default image and produced expected startup logs: `PASS`. Task 7 verified INFO / WARN / ERROR before and after scheduler start: `PASS`.
- Controlled undefined-instruction UsageFault: `PASS`. RTT captured `CFSR=0x00010000`, `FAULT PC=0x080002B6`, `Fault on thread defaultTask`, stacked registers, and CmBacktrace thread-stack output. Evidence: `06_Output/S01/RuntimeEvidence/Task9-UsageFault-CmBacktrace-2026-09-27.md`.
- Map / AXF / Listing correlation: `PASS`; the Thumb symbol is `0x080002B5`, and `DCW 0xDEAD` is at `0x080002B6`.
- Offline GDB correlation: `PASS`. The GDB executable and version were reused from the prior Bootloader project's local tool configuration (GNU GDB 10.2.90). With `set arm force-mode thumb`, GDB resolved `0x080002B6` to `diagnostics_fault_trigger_undefined+2` and decoded `ad de` as `udf #173`; `0x080002B8` is `bx lr`. Output: `06_Output/S01/RuntimeEvidence/Task9-GDB-Fault-Correlation-2026-09-27.txt`. GDB warned that `RW_IRAM1` is outside ELF loadable segments; the inspected Flash code and symbol were present.
- Temporary Fault configuration was restored byte-for-byte: `freertos.c` SHA-256 `9DB7FF78737BD3B735E8D19B554520963643FF800E43AD882AC5DFFC69A71992`; `diagnostics_config.h` SHA-256 `AC706C7ABB2379627181183939D1D412677DB8FADBF7249D7E3B0D3DAE6617C2`. `DIAG_FAULT_TEST_ENABLE` is `0U`; no trigger call remains on the normal task path.
- Task 9 result: `PASS` for Clean Rebuild, default-image Flash/Run, RTT startup, controlled Fault, and GDB / Map / AXF / Listing correlation.

## Diagnostics summary

- RTT startup and direct Fault output: `PASS`.
- Service Log → Platform Log → EasyLogger → RTT, including INFO / WARN / ERROR: `PASS`.
- CmBacktrace, Fault context, RTOS thread context, and unique Fault Handler ownership: `PASS`.
- Temporary Fault test cleanup and default-image recovery Build / Flash / Run: `PASS`.
- GDB / Map / AXF / Listing correlation: `PASS`.

## Additional configuration observation

- Pinout 基线标注 `PLL48CLK=48 MHz`；当前 `.ioc` 使用 HSI + PLL，`PLLQ=4`，记录的 `PLLQCLK=84 MHz`。
- Result: `PENDING` 设计意图核对。本次未改时钟配置，也未将其描述为已验证的硬件行为。

## Final result

- Code verification: `PASS` for the S01 implementation and planned build / diagnostic checks.
- Hardware verification: the S01 build, Flash, Run, RTT, EasyLogger, and controlled Fault checks are `PASS`; physical board identity, the Pinout / PLLQ discrepancy, and actual HSE frequency remain `PENDING` facts.
- S01 eligible for Review: `YES`, with the listed hardware facts and the historical intermittent J-Link failure cause explicitly open for the reviewer; S01 is not closed.
- Other recorded limitation: `.ioc` records CubeMX 6.8.1 while the installed CLI used for the single Generate is 6.8.1-RC4. The actual Generate produced no file differences and the following Clean Rebuild passed. RS485 direction behavior remains for its later hardware stage.

## Final Git status

- S01 source, configuration, and required documents are committed and pushed on `main`; the working tree is clean after delivery.
- Local CodeGraph / `.embeddedskills` state, Build Artifacts, and all `06_Output/` evidence remain excluded from Git. Machine-generated `JLinkLog.txt` and `.uvguix` were preserved under ignored `06_Output/S01/LocalConfigBackup/` and their tracked copies restored to the repository baseline.
- No AXF, HEX, MAP, Listing, temporary Fault image, or local tool path is part of the commit.
- `git -c core.whitespace=cr-at-eol diff --cached --check` reports upstream trailing whitespace / blank-EOF formatting in reused Library and Vendor files, including the Library `platform_types.h` family and third-party sources. Those files were not reformatted; `platform_types.h` SHA-256 remains identical to the frozen Library copy.
