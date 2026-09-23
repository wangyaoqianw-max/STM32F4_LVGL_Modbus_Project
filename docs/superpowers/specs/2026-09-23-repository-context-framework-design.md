# STM32F407VET6 仓库上下文框架设计

## 目标

为 `STM32F4_LVGL_Modbus_Project` 建立一套轻量的仓库级框架，首要目标是让人工和不同 Agent 能从仓库恢复项目状态、约束、当前阶段和下一步工作。框架复用 `STM32F4_Bootloader_OTA_Test_Project` 中成熟的上下文与阶段管理方法，按当前工程实际收敛工具体系。

## 当前基线

- 当前本地 `main` 已对齐并跟踪 `origin/main` 的种子提交 `59ece4cbcebcaf770073449a740404db0d97e561`；项目初始化提交尚未创建。
- 当前仓库原有的 `STM32F407VET6_工业控制从机综合项目_设计决策.md` 已按用户要求原样移动到 `00_Project/04_Decisions/`。初始化不得改写其内容；根 README 和项目上下文引用它作为现有设计基线。
- 该设计决策将工程准备列为阶段 0、F407 最小平台列为阶段 1。工程准备是正式功能阶段前的工作项，不额外虚构一个功能 Stage。
- `05_Tools` 当前只建立用途说明。暂不复制工具脚本、Toolkit、工具适配层、配置目录或自动化测试。

## 设计原则

1. 项目状态、阶段边界、硬件事实、设计决定和交接记录以仓库文件为准，不依赖聊天记录。
2. `PROJECT_CONTEXT.md` 提供快速入口；`00_Project/05_Status/current_status.md` 是活动阶段状态的唯一真值，二者保持一致。
3. 根级 `AGENTS.md` 只包含跨工具长期规则；具体工作由阶段角色和目标目录约束，不绑定 Codex、GPT 或其他 Agent 产品。
4. 不复制来源工程中的 F411 配置、板级代码、工具脚本、机器路径和阶段历史。
5. 工具按实际工作逐步加入。机器相关路径与生成物不得进入正式工程设计或提交内容。

## 仓库结构

```text
AGENTS.md
README.md
PROJECT_CONTEXT.md
00_Project/
  WORKFLOW.md
  00_Preparation/
  01_Requirements/
  02_Roadmap/
  03_Stages/_Template/
  04_Decisions/
  05_Status/current_status.md
01_Reference/
02_Hardware/
03_Firmware/
  AGENTS.md
  00_Doc/
  Application/
  Bootloader/
  Shared/
04_Test/
  Host/ Board/ Integration/ Test_Plans/ Reports/
05_Tools/README.md
06_Output/
```

### 根级入口

- `AGENTS.md`：指令优先级、上下文恢复顺序、修改范围、阶段和验证约束。
- `README.md`：项目定位、目录职责、快速开始和初始化路径；链接现有设计决策。
- `PROJECT_CONTEXT.md`：活动工作项、状态、分支、基线 Commit、当前角色、下一步和 Required Reading。

### 项目状态与阶段文档

- `00_Project/WORKFLOW.md` 定义 Design、Implementation、Verification、Review 和 Project Owner 的输入、允许动作、输出与阶段转换。
- 每个正式阶段使用 `00_Project/03_Stages/<stage_id>_<topic>/`，包含 `design.md`、`implementation_plan.md`、`handoff.md`、`review.md`。
- 阶段验证报告放入 `04_Test/Reports/Stages/<stage_id>/verification.md`，明确记录代码验证和硬件验证状态。
- `PROJECT_CONTEXT.md` 至少记录 Active Work Item、Status、Branch、Baseline Commit、Current Role、Next Action 和 Required Reading。没有真实 Commit 时写 `Not created yet`，不猜测哈希。
- `current_status.md` 保存活动阶段状态、负责人、验收入口和下一步；更新状态时同步上下文入口。

### Agent 使用约定

- Agent 按 Design、Implementation、Verification 或 Review 角色工作；Project Owner 负责批准需求和设计、解决硬件事实冲突、接受硬件结论和关闭阶段。
- 接手任务先读 `AGENTS.md`、`README.md`、`PROJECT_CONTEXT.md`，再按 Required Reading 读取当前阶段文档、目标目录规则及相关资料。
- Implementation 只修改已批准计划明确列出的范围；Verification 按验收条件留下可回读证据；Review 对照需求、计划、差异和证据审查。
- 角色可由任何兼容仓库工作流的 Agent 或人工承担。是否拆分给多个 Agent 按任务依赖和资源冲突决定，不要求每项工作都并行。
- 工具切换前更新 handoff、项目上下文和状态入口，记录实际 Commit；未写入仓库的聊天结论不作为冻结工程事实。

### 工具与输出

- 初始 `05_Tools/README.md` 只说明目录职责、添加工具的原则和当前尚未建立自动化入口的状态。
- 后续由具体需求驱动工具目录扩展；可以逐步增加 `Config`、`Scripts`、`Workflows` 或 `Adapters`，不预设完整 Toolkit 架构。
- 工具稳定后再定义统一入口；本机路径通过本地配置管理并加入 Git 忽略规则，提交可移植的示例配置。
- 编译产物、固件包、调试日志等写入 `06_Output` 并默认忽略；阶段验收结论和正式报告保存在 `04_Test/Reports`。
- 只有共享同一设备、Probe、串口、构建输出或结果路径时才对工具操作互斥；工具链动作成功不能替代功能验收。

## 工程准备状态

初始化后的项目状态为 `PREPARATION / IN_PROGRESS`，Current Role 为 `Project Owner`。下一步依据现有设计决策中的阶段 0，整理原理图、Pinout、器件资料、外设/DMA/IRQ 资源、开发环境版本和未决事项。准备期间只记录已确认事实、假设和未知项，不从 F411 来源工程推导新板硬件事实。

阶段 0 完成后，再依据当前路线进入 F407VET6 最小平台设计与实施；具体工具脚本在对应实施计划明确且工具环境核实后再加入。

## 初始化验收条件

1. 根目录可通过 README 找到项目基线、当前状态和工作流入口。
2. Agent 按固定阅读顺序可恢复当前工作项、范围和下一步。
3. 阶段模板覆盖设计、计划、交接和评审；验证报告位置明确。
4. 工具板块保持轻量，不迁入来源工程的工具实现。
5. 输出物和本机配置有明确的 Git 忽略边界。
6. 现有设计决策文件位于 `00_Project/04_Decisions/`，内容保持原样并纳入项目入口引用。
7. 初始化完成后建立首个项目提交，并推送到已配置的 `origin`；推送前检查远端分支，禁止强制覆盖远端历史。

## 不在本次范围

- 编写、移植或构建任何固件代码。
- 复制来源工程的工具脚本或实现具体的 Build、Flash、RTT、GDB、YMODEM 自动化。
- 修改或移动现有设计决策文档。
- 冻结阶段 0 尚未确认的硬件资源、外设映射、工具版本或引脚事实。
