# Project Context

本文件是恢复当前工程上下文的入口。活动状态以 `00_Project/05_Status/current_status.md` 为准。

## Context Metadata

- Active Work Item: `S01 Application 工程初始化与诊断基础`
- Status: `READY_FOR_IMPLEMENTATION`
- Branch: `main`
- Baseline Commit: `fd197f695f0742627470cb54068aadc72a238477`
- Current Role: `Project Owner / Implementation Role`
- Stage Docs: `00_Project/03_Stages/S01_Application工程初始化与诊断基础/`
- Next Action: 从 S01 Implementation Plan Task 1 开始施工；冻结基线后先独立跑通 `Build → J-Link Flash → Run` 最小工具链，再整理 Keil 输出、迁入最小五层框架和 Diagnostics 资产。

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
- 当前 Keil `.uvprojx` 还未按正式输出规范完成 `Objects/Listsings` 整理，属于 S01 Task 2。

## 当前冻结约束

- `platform_types.h` 是项目基础类型资源，直接使用 Library 当前文件，不修改。
- 不为了符合新的类型偏好而在 S01 重构既有五层基础资产。
- CubeMX 生成的 `Core/`、`Drivers/`、`Middlewares/` 保持原位置。
- 第三方源码进入 `05_Vendors/` 后保持原目录/版权/License，项目适配代码放在自研层。
- Fault 诊断路径保持最小依赖，不依赖 EasyLogger 才能输出关键 Fault 信息。
- 不提前实现 S02 以后硬件驱动和业务功能。
- 未确认的硬件信息保持 `Unknown / TO_VERIFY`。

## 当前验证状态

- S01 Code Verification: `NOT_RUN`
- S01 Hardware Verification: `NOT_RUN`
- Evidence: `04_Test/Reports/Stages/S01/verification.md`
