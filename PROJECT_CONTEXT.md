# Project Context

本文件是恢复当前工程上下文的入口。活动状态以 `00_Project/05_Status/current_status.md` 为准。

## Context Metadata

- Active Work Item: `PREPARATION`
- Status: `IN_PROGRESS`
- Branch: `main`
- Baseline Commit: `59ece4cbcebcaf770073449a740404db0d97e561`（仓库初始化前的远端种子提交）
- Current Role: `Project Owner`
- Next Action: 整理阶段 0 的硬件事实、外设资源、资料索引、工具版本和未决事项；当前手边没有开发板，板级试验待设备具备后执行

## Required Reading

1. `AGENTS.md`
2. `README.md`
3. `00_Project/WORKFLOW.md`
4. `00_Project/05_Status/current_status.md`
5. `00_Project/04_Decisions/STM32F407VET6_工业控制从机综合项目_设计决策.md`
6. `00_Project/00_Preparation/README.md`
7. `05_Tools/README.md`

## 当前约束

- 本工程以 STM32F407VET6 新开发板为目标；阶段 0 未确认的硬件信息保持为未知。
- 当前仍处于工程准备，不在本上下文中宣称已完成固件工程、工具链或板级验证。
- `05_Tools` 约定优先使用本机已安装的 Embedded Skills，不复制 Skill 工具脚本；工程配置待阶段 0 确认后再建立。
- 当前手边没有开发板，尚无板级试验或硬件验证结论；取得设备后按阶段计划补验。
- 现有设计决策文档位于 `00_Project/04_Decisions/`，是当前架构与实施路线的基线。
