# Project Context

本文件是恢复当前工程上下文的入口。活动状态以 `00_Project/05_Status/current_status.md` 为准。

## Context Metadata

- Last Closed Stage: `S01A 本机工具 Skill 接入与入口整理`
- Last Closed Stage Status: `CLOSED`
- Branch: `main`
- S01 Implementation Commit: `eabc22ed58e770a80f41dcd94ce427710a658c44`
- S01A Implementation Commit: `7a942b57d75e2c1c2c5c0bd2238141b12898cf0a`
- Active Stage: `S02 板级基础能力 / Platform Bring-up`
- Active Stage Status: `IN_PROGRESS`
- Current Role: `Verification / Independent Review`
- S02 Design Baseline Commit: `0ffe747eeb0a1e7e2c6a088e2b402bbc57965f44`
- Next Work Item: `S02 独立 Review 与阶段关闭`
- Next Action: 计划内代码、Clean Rebuild 和 GPIO / I2C ACK / USART1 Smoke 已完成；测试入口已清除，等待 Review PASS 后关闭 S02 并切换到 S03。

## Required Reading

1. `AGENTS.md`
2. `README.md`
3. `00_Project/WORKFLOW.md`
4. `00_Project/05_Status/current_status.md`
5. `00_Project/03_Stages/S02_板级基础能力与Platform_Bring-up/design.md`
6. `00_Project/03_Stages/S02_板级基础能力与Platform_Bring-up/implementation_plan.md`
7. `00_Project/03_Stages/S01_Application工程初始化与诊断基础/handoff.md`
8. `00_Project/03_Stages/S01_Application工程初始化与诊断基础/review.md`
9. `04_Test/Reports/Stages/S01/verification.md`
10. `00_Project/02_Roadmap/development_roadmap.md`
11. `02_Hardware/Pinout/STM32F407VET6_CubeMX_外设与引脚配置基线.md`
12. `00_Project/04_Decisions/STM32F407VET6_工业控制从机综合项目_设计决策.md`
13. `03_Firmware/00_Doc/Keil工程与构建输出规范.md`
14. `03_Firmware/00_Doc/RTT_CmBacktrace_AI移植指南.md`
15. `03_Firmware/00_Doc/嵌入式C代码规范.md`
16. `00_Project/03_Stages/S01A_本机工具Skill接入与入口整理/design.md`
17. `00_Project/03_Stages/S01A_本机工具Skill接入与入口整理/implementation_plan.md`
18. `05_Tools/README.md`
19. `00_Project/03_Stages/S01A_本机工具Skill接入与入口整理/handoff.md`
20. `00_Project/03_Stages/S01A_本机工具Skill接入与入口整理/review.md`
21. `04_Test/Reports/Stages/S01A/verification.md`

外部复用资产：

- `wangyaoqianw-max/Embedded_Engineering_Library`
- S01 冻结参考 Commit：`8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0`

## 当前 Application 基线

- 目标 MCU：STM32F407VET6。
- 工程：`03_Firmware/Application/STM32F407_APP/`。
- 当前系统主时钟使用 HSI + PLL 得到 168 MHz；HSE 实际频率仍为 `TO_VERIFY`。
- CubeMX 已配置 FreeRTOS、SWD、SPI1、USART1/2/3、6 路 UART DMA、USART IRQ、FSMC、XPT2046 GPIO 和软件 I2C GPIO。
- Keil Output / Listing 已分别规范到 `MDK-ARM/Objects/` 和 `MDK-ARM/Listings/`。
- 五层基础框架已进入工程。
- `platform_types.h` 保持冻结 Library Blob `a2d23e8575f31494e55548bde62c15e7407055b9`，后续不得因风格偏好顺手修改。
- FreeRTOS Kernel `tasks.c` 未修改；CmBacktrace 任务上下文通过 additions 扩展接入。
- Diagnostics：
  - RTT v7.92；
  - EasyLogger v2.2.99；
  - Service Log / Platform Log；
  - CmBacktrace v1.5.0；
  - 项目 Fault Adapter；
  - 四类 Cortex-M Fault Handler 由项目 Fault 汇编统一接管。
- Fault 测试默认关闭：`DIAG_FAULT_TEST_ENABLE=0U`。

## S01 已验证能力

```text
Clean Rebuild
    ↓
J-Link Flash
    ↓
Reset / Run
    ↓
FreeRTOS Runtime
    ↓
RTT
├── EasyLogger / Service Log
└── CmBacktrace / Fault
    ↓
GDB / Map / AXF / Listing
```

S01 Code Verification 与阶段要求内的工具链/板级验证均为 `PASS`。CubeMX Generate 实际回归一次，前后 1,185 个文件无差异，随后 Clean Rebuild `PASS`。

## 架构与所有权约束

- 依赖方向保持 `APP → Service → Platform → Impl → HAL / RTOS / Vendor`。
- Service 不直接依赖 HAL Handle、GPIO、DMA 或具体 STM32 外设。
- CubeMX Generated Code 主要承担初始化和必要桥接，业务逻辑不堆积在生成文件。
- 第三方源码保持版本、License 和原风格，项目适配放在自研层。
- Fault 路径保持最小依赖，关键故障输出不依赖 EasyLogger 正常链。
- S01 没有提前引入 UART Service、RingBuffer 和设备业务驱动；这些按后续阶段需求复用。

## S02 已冻结设计边界

S02 Design / Implementation Plan 已批准；Platform / Impl 接入、F407 Binding 与计划内验证已完成，当前等待 Review。

目标链路：

```text
CubeMX / HAL Resource
        ↓
STM32F407 Impl Backend
        ↓
Board / HAL Binding
        ↓
Platform API
```

本阶段允许迁入 GPIO、Delay、Software I2C、SPI、UART、IRQ、Reset 及其 STM32F4 Backend；Watchdog 暂缓。

验证边界：

- GPIO：最小物理 Smoke；
- Software I2C：PB6/PB7 对板载 AT24C02 做地址 ACK Probe，不写 EEPROM；
- UART：从 USART1/2/3 中选择最方便的一路做 Blocking TX/RX Smoke；
- SPI：只完成 Binding / Build，W25Q128 JEDEC/Read 留 S03；
- DMA：不新增 `platform_dma`，UART DMA Runtime 留 S04；
- FMC：不新增 `platform_fmc`，LCD/FMC 实机时序留 S06；
- Reset：允许迁入，但不单独安排人工板测。

S02 不建立完整 Driver / Service 初始化链，也不提前引入 W25Q128、DHT20、AT24C02 完整 Driver、UART Service、RingBuffer、LVGL、Modbus 或 OTA。

## Carry-forward / TO_VERIFY

- HSE 实际晶振频率；
- 实物板卡身份独立确认；
- PB6/PB7 实际外部上拉；
- HC-05 实际 UART 参数；
- Modbus 正式 UART 参数；
- RS485 收发方向控制；
- LCD/FSMC 实机时序；
- Pinout 文档中 PLL48 描述与当前未使用 PLL48 域配置之间的一致性；
- J-Link 历史间歇连接失败根因，仅在后续复现时继续调查。

这些事项不能自动当成已确认硬件事实。

## 下一步

S02 代码和计划内验证已完成，具体证据见：

- `04_Test/Reports/Stages/S02/verification.md`
- `00_Project/03_Stages/S02_板级基础能力与Platform_Bring-up/handoff.md`
- `00_Project/03_Stages/S02_板级基础能力与Platform_Bring-up/review.md`（Review 后建立）

当前待办是独立 Review。Review PASS 后关闭 S02，下一工作项切换为 `S03 W25Q128 与中文字库基础`；完整设备初始化和 Service 仍按后续阶段设计。
