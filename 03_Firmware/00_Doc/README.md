# 固件设计文档

`03_Firmware/00_Doc` 保存跨阶段有效的固件工程规范和迁移参考。具体阶段的目标、计划、交接和 Review 仍放在 `00_Project/03_Stages/`。

当前正式参考：

| 文档 | 用途 |
| --- | --- |
| `Keil工程与构建输出规范.md` | 规定 Keil Output/Listing、Git 边界、Build Artifact 与交付物管理 |
| `RTT_CmBacktrace_AI移植指南.md` | 基于 F407/F411 Clean/移植对照实验指导 RTT + CmBacktrace 接入和再生成检查 |
| `嵌入式C代码规范.md` | 项目自研 C 代码设计、分层、并发、日志和 Review 规则 |

使用原则：

- 文档约束不用于无需求重构已经冻结的基础资产；
- S01 明确保留并原样使用 Embedded Engineering Library 的 `platform_types.h`；
- 第三方和 CubeMX Generated Code 保持其原始风格，项目通过 Adapter/Impl 隔离；
- 未冻结内容继续标记为假设或待验证。

S01 设计与施工计划：

`00_Project/03_Stages/S01_Application工程初始化与诊断基础/`
