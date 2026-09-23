# STM32F4_LVGL_Modbus_Project
个人工业控制模拟项目

## 项目基线

- MCU：STM32F407VET6 新开发板。
- 项目架构、硬件事实、复用边界和分阶段路线以[工程设计决策](00_Project/04_Decisions/STM32F407VET6_工业控制从机综合项目_设计决策.md)为当前基线。
- 当前工作项：工程准备与硬件事实基线。尚未进入固件施工阶段。

## 快速恢复上下文

接手工作时按以下顺序读取：

1. `AGENTS.md`：仓库级协作规则。
2. `PROJECT_CONTEXT.md`：当前工作项、状态、下一步和必读资料。
3. `00_Project/WORKFLOW.md`：阶段和角色约定。
4. `00_Project/05_Status/current_status.md`：活动状态真值。
5. 当前工作项的设计、计划、交接和目标目录规则。

## 目录职责

| 目录 | 内容 |
| --- | --- |
| `00_Project` | 工程准备、需求入口、路线图、阶段文档、设计决策和状态 |
| `01_Reference` | 芯片、外设、协议和开发工具的原始参考资料 |
| `02_Hardware` | 原理图、Pinout 和硬件软件接口事实 |
| `03_Firmware` | Application、Bootloader、共享代码及固件规范 |
| `04_Test` | Host、Board、Integration 测试方案和验证证据 |
| `05_Tools` | 后续逐步建立的本机工具入口和自动化 |
| `06_Output` | 编译产物、固件包和运行日志等生成物，不提交 Git |

## 当前阶段

按工程设计决策的阶段 0 收集原理图、Pinout、器件资料、外设/DMA/IRQ 资源、工具版本和待确认事项。准备完成后再进入阶段 1 的 F407VET6 最小平台。具体状态见 `PROJECT_CONTEXT.md` 和 `00_Project/05_Status/current_status.md`。

## 初始化与协作

- Agent 按 Design、Implementation、Verification、Review 角色工作；角色由任务和阶段决定，不绑定特定 AI 产品。
- 工程事实和正式决定写入仓库文档并通过 Git 留痕；交接时更新阶段 `handoff.md`、`PROJECT_CONTEXT.md` 和 `current_status.md`。
- `05_Tools` 暂不迁移来源工程工具。实际需要确定后再逐项增加，不预设完整工具链。
