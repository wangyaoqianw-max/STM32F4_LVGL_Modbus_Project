# Project Context

本文件是恢复当前工程上下文的入口。活动状态以 `00_Project/05_Status/current_status.md` 为准。

## Context Metadata

- Active Work Item: `S01 Application 工程初始化与诊断基础`
- Status: `READY_FOR_REVIEW`
- Branch: `main`
- Baseline Commit: `fd197f695f0742627470cb54068aadc72a238477`
- Execution Source Commit: `0a8d61383564d62168431676c86c799c4abd240b`
- Current Role: `Implementation / Verification Complete`
- Stage Docs: `00_Project/03_Stages/S01_Application工程初始化与诊断基础/`
- Next Action: Review S01 against its design and verification evidence; assess the remaining hardware facts recorded as PENDING.

## Required Reading

1. `AGENTS.md`
2. `README.md`
3. `00_Project/WORKFLOW.md`
4. `00_Project/05_Status/current_status.md`
5. `00_Project/03_Stages/S01_Application工程初始化与诊断基础/design.md`
6. `00_Project/03_Stages/S01_Application工程初始化与诊断基础/implementation_plan.md`
7. `03_Firmware/00_Doc/Keil工程与构建输出规范.md`
8. `03_Firmware/00_Doc/RTT_CmBacktrace_AI移植指南.md`
9. `03_Firmware/00_Doc/嵌入式C代码规范.md`
10. `02_Hardware/Pinout/STM32F407VET6_CubeMX_外设与引脚配置基线.md`
11. `00_Project/04_Decisions/STM32F407VET6_工业控制从机综合项目_设计决策.md`

外部复用资产：

- `wangyaoqianw-max/Embedded_Engineering_Library`
- S01 参考 Commit：`8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0`

## 当前工程事实

- 目标 MCU：STM32F407VET6。
- Application 工程：`03_Firmware/Application/STM32F407_APP/`。
- CubeMX 已建立 FreeRTOS、SWD、SPI1、USART1/2/3、DMA/IRQ、FSMC、XPT2046 GPIO、软件 I2C 等静态配置基线。
- 当前系统主时钟使用 HSI + PLL 得到 168 MHz；HSE 实际晶振频率仍未确认。
- `MDK-ARM/Objects/` and `MDK-ARM/Listings/` verified by Clean Rebuild: AXF/HEX in Objects; MAP and 66 C/assembly `.lst` files in Listings after Task 9.

## S01 执行进度

- Task 1–6 已完成并留有构建、目录、架构和诊断证据；平台基础类型按冻结 Library Blob 原样保留，FreeRTOS Kernel 未修改。
- Task 7 EasyLogger / Service Log 正常日志链和 INFO/WARN/ERROR RTT 验证：`PASS`；临时标记已移除。
- Task 8 实际运行一次 CubeMX CLI Generate：前后 1,185 个文件无哈希差异，随后 Clean Rebuild `PASS`。用户要求后没有重复 Generate。
- Task 9 最终默认 Clean Rebuild `PASS`，0 Error / 0 Warning；AXF/HEX 在 Objects，MAP 和 66 个 `.lst` 在 Listings。
- 最终默认镜像 Clean Rebuild `PASS`，0 Error / 0 Warning；J-Link / DAPLink/probe-rs Build-independent run 与 RTT 初始化输出均 `PASS`。受控 UsageFault 与 Map/AXF/Listing/GDB Thumb 指令对照均 `PASS`。
- Fault 临时测试已恢复为默认关闭：`DIAG_FAULT_TEST_ENABLE=0U`，正常任务入口无测试触发调用；最终默认镜像未输出测试标记。
- 最近一次 J-Link Connect / 校验烧录 / Reset / Run / RTT：`PASS`，VTref 3.301 V；此前间歇连接失败原因未知。
- S01 状态为 `READY_FOR_REVIEW`；实物板卡身份、Pinout / PLLQ 差异及 HSE 实际频率仍标记为 `PENDING`，交由 Review 评估；阶段尚未关闭。详见 `04_Test/Reports/Stages/S01/verification.md`。
## 当前冻结约束

- `platform_types.h` 是项目基础类型资源，直接使用 Library 当前文件，不修改。
- 不为了符合新的类型偏好而在 S01 重构既有五层基础资产。
- CubeMX 生成的 `Core/`、`Drivers/`、`Middlewares/` 保持原位置。
- 第三方源码进入 `05_Vendors/` 后保持原目录/版权/License，项目适配代码放在自研层。
- Fault 诊断路径保持最小依赖，不依赖 EasyLogger 才能输出关键 Fault 信息。
- 不提前实现 S02 以后硬件驱动和业务功能。
- 未确认的硬件信息保持 `Unknown / TO_VERIFY`。

## 当前验证状态

- S01 Code Verification: `PASS`, including the Task 9 offline GDB / Map / AXF / Listing Fault PC correlation.
- S01 Hardware Verification: S01 Clean Rebuild, Flash / Run, RTT / EasyLogger, and controlled Fault output are `PASS`; physical board identity and unresolved hardware facts remain `PENDING`.
- J-Link：最近一次 Connect / Flash 校验 / Reset / Run / RTT：`PASS`，VTref `3.301 V`；历史间歇失败原因未知。
- DAPLink / probe-rs：Task 7 日志等级与 Task 9 默认镜像运行输出 `PASS`。
- CubeMX Regenerate：已实际执行一次，1,185 个文件无差异，后续 Clean Rebuild `PASS`；未按用户要求重复执行。
- S01 进入 Review：`YES`；Review 需评估仍为 `PENDING` 的板卡身份、Pinout / PLLQ 差异、HSE 频率及历史 J-Link 间歇失败原因。S01 尚未关闭。
- Evidence: `04_Test/Reports/Stages/S01/verification.md`
