# 阶段式工程工作流

## 目的

用仓库内的状态、阶段文档和验证证据保存工程上下文，使不同 Agent、IDE 和人工维护者能从当前仓库状态继续工作。流程定义角色与交付物，不绑定具体 AI 或开发工具。

## 正式上下文

正式上下文由 `PROJECT_CONTEXT.md`、项目需求与设计决策、路线图、当前阶段文档、验证报告和 Git 历史共同组成。`00_Project/05_Status/current_status.md` 是活动状态的唯一真值；上下文入口必须与其同步。

## 工程准备与正式阶段

工程准备 `PREPARATION` 是第一个正式设计阶段之前的前置工作项，用于整理资料、核对硬件事实、确认开发环境和显式记录未知项。准备完成后再建立第一个正式设计阶段。

每个正式阶段保存在 `00_Project/03_Stages/<stage_id>_<topic>/`，包含：

- `design.md`：目标、范围、方案、依赖和验收条件；
- `implementation_plan.md`：按顺序执行的修改与验证步骤；
- `handoff.md`：基线、输入、完成项、输出、未解决项和下一步；
- `review.md`：对照需求、变更和验证证据给出的评审结论。

阶段验证报告放在 `04_Test/Reports/Stages/<stage_id>/verification.md`。

## 状态转换

```text
DRAFT
→ DESIGN_APPROVED
→ READY_FOR_IMPLEMENTATION
→ IN_PROGRESS
→ READY_FOR_VERIFICATION
→ READY_FOR_REVIEW
→ CLOSED
```

发现外部依赖或必要信息缺失时，可进入 `BLOCKED`；评审发现明确问题时进入 `CHANGES_REQUESTED`，完成修正后回到 `IN_PROGRESS`。不得通过修改验收条件隐藏失败或在缺少证据时关闭阶段。

## 角色职责

### Design Role

读取需求、路线图、相关决策和前序交接；输出设计、实施计划及施工输入。Project Owner 批准后才可进入实施。

### Implementation Role

读取根规则、上下文、当前阶段文档、目标目录规则和相关资料；只修改已批准计划范围内的内容，并记录实际改动和结果。

### Verification Role

以阶段验收条件为依据执行计划内验证，将命令、结果、日志和未验证项写入报告；代码验证与硬件验证分别判定。

### Review Role

检查需求、设计、计划、变更、交接和验证报告，给出通过、返工或阻塞结论；不能用评审代替缺失的验证证据。

### Project Owner

确认需求和设计，裁决硬件事实冲突，批准范围外操作，接受真实硬件验收并关闭阶段。

## 工具切换和交接

离开当前工具或 Agent 前，更新阶段 `handoff.md`、`PROJECT_CONTEXT.md` 和 `current_status.md`，记录当前分支、实际基线/变更 Commit、完成项、验证状态和下一步。新工具接手后先核对工作区和 Git 状态，再按上下文中的 Required Reading 恢复工程状态。

角色可由任何能访问仓库并遵守本流程的人工或 Agent 承担。只有子任务独立、输入输出清楚且不争用同一板卡、Probe、串口、构建输出或文件时才并行安排。
