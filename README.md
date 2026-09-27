# STM32F4_LVGL_Modbus_Project
个人工业控制模拟项目

## 项目基线

- MCU：STM32F407VET6 新开发板。
- 项目架构、硬件事实、复用边界和分阶段路线以[工程设计决策](00_Project/04_Decisions/STM32F407VET6_工业控制从机综合项目_设计决策.md)为当前基线。
- S00 工程准备和 S01 Application 工程初始化与诊断基础已经完成。
- 下一工作项为 S02 板级基础能力 / Platform Bring-up；S02 尚未冻结设计，先讨论 Design / Implementation Plan。

## 快速恢复上下文

接手工作时按以下顺序读取：

1. `AGENTS.md`：仓库级协作规则。
2. `PROJECT_CONTEXT.md`：已完成阶段、当前状态、下一步和必读资料。
3. `00_Project/WORKFLOW.md`：阶段和角色约定。
4. `00_Project/05_Status/current_status.md`：活动状态真值。
5. S01 `handoff.md` / `review.md`，以及下一阶段正式建立后的设计和计划。

## 目录职责

| 目录 | 内容 |
| --- | --- |
| `00_Project` | 工程准备、需求入口、路线图、阶段文档、设计决策和状态 |
| `01_Reference` | 芯片、外设、协议和开发工具的原始参考资料 |
| `02_Hardware` | 原理图、Pinout 和硬件软件接口事实 |
| `03_Firmware` | Application、Bootloader、共享代码及固件规范 |
| `04_Test` | Host、Board、Integration 测试方案和验证证据 |
| `05_Tools` | 本机工具入口和自动化 |
| `06_Output` | 编译产物、固件包和运行日志等生成物，不提交 Git |

## 当前工程能力

S01 已形成可继续施工的 Application 母工程：

```text
STM32F407 CubeMX / HAL
        ↓
Keil Build
        ↓
J-Link Flash / Run
        ↓
FreeRTOS
        ↓
五层架构基础
        ↓
RTT
├── EasyLogger / Service Log
└── CmBacktrace / Fault
```

阶段验证覆盖 Clean Rebuild、Flash/Run、RTT、正常日志、受控 Fault、GDB/Map/AXF/Listing 对照和一次 CubeMX Generate 回归。

具体状态、未确认硬件事实和下一阶段入口见 `PROJECT_CONTEXT.md` 与 `00_Project/05_Status/current_status.md`。

## 初始化与协作

- Agent 按 Design、Implementation、Verification、Review 角色工作；角色由任务和阶段决定，不绑定特定 AI 产品。
- 工程事实和正式决定写入仓库文档并通过 Git 留痕；交接时更新阶段 `handoff.md`、`PROJECT_CONTEXT.md` 和 `current_status.md`。
- 复用优先来自 `Embedded_Engineering_Library`，但任何资产进入新阶段前仍需检查目标 MCU、硬件资源、依赖、License 和当前验证边界。
