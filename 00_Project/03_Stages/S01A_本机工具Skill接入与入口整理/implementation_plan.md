# S01A 本机工具 Skill 接入与入口整理 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use `superpowers:subagent-driven-development` or `superpowers:executing-plans` to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 在 `05_Tools` 建立可复用的项目级 Embedded Skills 配置、首次初始化入口和 Keil/J-Link/GDB 调用速查，并将 S01A 正式纳入 S01 与 S02 之间。

**Architecture:** 仓库保存相对路径的项目配置示例；一个 PowerShell 初始化脚本仅在 `.embeddedskills/config.json` 不存在时复制示例。构建、烧录、RTT 和 GDB 动作继续由现有 Skills 执行，用户级工具路径留在各自本机 Skill 配置。

**Tech Stack:** PowerShell、JSON、Markdown、Git。

**Spec:** `00_Project/03_Stages/S01A_本机工具Skill接入与入口整理/design.md`

## Metadata

- Stage: `S01A 本机工具 Skill 接入与入口整理`
- Status: `CLOSED`
- Design Baseline Commit: `c27f8af01be6e4910247a42078c7ec255fa15c98`
- Owner: `Project Owner`

## Scope and constraints

仅实施已批准设计中列出的项目配置示例、首次配置初始化脚本、Skill 入口说明、阶段路线/状态同步和本阶段交付文档。S01 保持关闭；S02 的硬件 Bring-up 设计与实现不在本计划范围内。用户级工具路径、本机 `.embeddedskills` 状态和板级操作均保持在设计规定的边界之外。

## Global Constraints

- S01 保持 `CLOSED`；S01A 安排在 S01 之后、S02 之前；S02 继续保持下一固件阶段且不重新编号。
- 不复制或镜像 Skill 包、工具程序或 Skill 通用脚本。
- 项目配置只使用仓库相对路径；本机可执行文件路径、探针序列号、凭据和本机运行状态不得进入 Git。
- 不覆盖、合并或提交现有 `.embeddedskills/config.json` / `.embeddedskills/state.json`。
- GDB 通过 `jlink` Skill 使用；Keil 和 J-Link/GDB 工具路径由各自用户级 Skill 配置管理。
- 不执行构建、清理、烧录、复位、在线调试、串口发送或其他板级操作；不新增或运行测试。
- 完成阶段评审后，只提交本计划列出的项目文件，并按用户要求推送当前分支。

## Review Focus

- 已存在的 `.embeddedskills/config.json` 必须保持原样；初始化脚本只能对缺失文件执行首次创建。
- 共享配置示例不得带入用户级可执行文件路径、probe-rs 的本机 Probe ID、序列号或凭据。
- `STM32F407VE` 只作为当前 J-Link Skill 工程目标参数，文档不得据此宣称实物板卡身份已独立确认。
- GDB 的 server/executable 路径属于用户级配置；项目速查不得制造第二套配置源。
- 路线图、上下文、状态、交接与评审结论必须一致；S01 继续关闭，S02 硬件事实保持原状。

## Files and responsibilities

| File | Responsibility |
| --- | --- |
| `05_Tools/Config/embeddedskills.config.example.json` | 保存可共享的 Keil Target/工程路径/日志目录及 J-Link 目标连接参数。 |
| `05_Tools/Scripts/Initialize-EmbeddedSkillsConfig.ps1` | 从示例首次创建本机忽略的 `.embeddedskills/config.json`，已有文件时安全退出。 |
| `05_Tools/README.md` | 说明项目配置边界、常用 Skill 入口和硬件操作前提。 |
| `00_Project/02_Roadmap/development_roadmap.md` | 将 S01A 插入 S01 与 S02 之间，不改变后续阶段编号。 |
| `README.md`, `PROJECT_CONTEXT.md`, `00_Project/05_Status/current_status.md` | 同步当前已关闭阶段、下一工作项和恢复上下文。 |
| `00_Project/03_Stages/S01A_本机工具Skill接入与入口整理/handoff.md` | 保存 S01A 基线、交付、验证边界和下一步。 |
| `00_Project/03_Stages/S01A_本机工具Skill接入与入口整理/review.md` | 保存独立评审结论和阶段关闭决定。 |
| `04_Test/Reports/Stages/S01A/verification.md` | 记录配置/文档静态核对，以及未执行的板级验证。 |

## Tasks

### Task 1: 建立可复用 Skill 项目配置入口

**Files:**

- Create: `05_Tools/Config/embeddedskills.config.example.json`
- Create: `05_Tools/Scripts/Initialize-EmbeddedSkillsConfig.ps1`

**Interfaces:**

- 配置示例提供 `keil.project`、`keil.target`、`keil.log_dir`，以及 `jlink.device`、`jlink.interface`、`jlink.speed`。
- 初始化脚本无参数；仓库根目录从 `$PSScriptRoot` 推导。目标为仓库根目录下 `.embeddedskills/config.json`。
- 若目标不存在，创建 `.embeddedskills/`（必要时）并复制示例；若目标已存在，打印“不覆盖”提示并正常退出，不写入或合并。

- [x] **Step 1: 创建项目配置示例**，填入已确认的相对 Keil 工程路径、Target `STM32F407_APP`、日志目录 `06_Output/ToolSkills/BuildLogs`，以及当前 J-Link 参数 `STM32F407VE / SWD / 1000`。
- [x] **Step 2: 创建首次配置初始化脚本**，用 `Split-Path` / `Join-Path` 基于脚本位置解析仓库根目录，先检查目标文件；只有目标缺失时创建目录并调用 `.NET File.Copy`，明确关闭覆盖。
- [x] **Step 3: 静态检查配置和脚本**，确认字段符合当前 `keil` / `jlink` Skill 项目配置契约；确认脚本不包含 `-Force`、删除、合并、工具启动或用户级配置写入逻辑。
- [x] **Step 4: 检查配置边界**，确认示例只包含相对工程路径和可共享项目参数，不含机器绝对路径、Probe ID、序列号或凭据；不运行脚本，以保留当前本机配置。

**Verification:** 用 PowerShell JSON parser 解析配置示例；用 PowerShell AST parser 静态解析初始化脚本；通过当前 .NET 方法元数据确认 `File.Copy(string, string, bool)` 可用，并回读脚本的 `overwrite = false` 与 `$ErrorActionPreference = 'Stop'`。已有目标在首次检查时正常退出；若目标在检查后才出现，复制操作明确失败且不覆盖；其他目录/复制错误也停止在成功提示前。检查 `.embeddedskills/` 仍被忽略；不读取或修改现有本机配置，不执行 Skills 工具动作。

**Output:** 可提交的项目配置模板和只负责首次创建的安全入口脚本。

### Task 2: 更新 05_Tools 使用说明

**Files:**

- Modify: `05_Tools/README.md`

**Interfaces:**

- 说明初始化入口：`powershell -NoProfile -File .\05_Tools\Scripts\Initialize-EmbeddedSkillsConfig.ps1`。
- 常见单项操作对应 `/keil` 和 `/jlink`；GDB 源码级调试对应 `/jlink gdb`；整条流程仅在用户明确要求时使用 `/workflow`。

- [x] **Step 1: 替换过时的配置说明**，删除“暂不创建 `.embeddedskills/config.json`”的旧阶段结论，改为示例文件与首次初始化脚本的用法，并明确本机覆盖仍被 Git 忽略。
- [x] **Step 2: 增加调用速查**，按工程扫描/Target/Build、J-Link info/flash/RTT、GDB 源码级调试列出对应 Skill 和输入产物；说明 GDB 使用 Keil 生成的 AXF。
- [x] **Step 3: 收敛工作流规则**，注明单项任务使用对应 Skill，`workflow` 只用于用户明确要求的多步骤编排；补充同一 Probe 的 J-Link/RTT/GDB 所有权约束。
- [x] **Step 4: 修正硬件状态措辞**，将“当前手边没有开发板”改为基于板卡/探针/参数是否可用的条件式操作边界，不把 S01 的历史验证写成当前设备可用承诺。
- [x] **Step 5: 回读 README**，确认没有复制 Skill 脚本/工具的指引、机器路径或与根项目状态冲突的结论。

**Verification:** 对照当前已安装 `keil`、`jlink`、`workflow` Skill 的调用和参数说明；逐项检查 README 中路径、输出位置和硬件边界。

**Output:** 与 S01 已完成状态及 S01A 设计一致的 `05_Tools/README.md`。

### Task 3: 同步阶段路线与项目上下文

**Files:**

- Modify: `00_Project/02_Roadmap/development_roadmap.md`
- Modify: `README.md`
- Modify: `PROJECT_CONTEXT.md`
- Modify: `00_Project/05_Status/current_status.md`

**Interfaces:**

- S01A 是 S01 后、S02 前的短支撑阶段；核心 S02–S12 阶段编号保持不变。
- 本任务开始时激活 S01A；S01A 关闭后的下一工作项恢复为 S02；不得改写 S01 的验证、评审或关闭结论。

- [x] **Step 1: 更新粗粒度路线图**，在 S01 与 S02 之间增加 S01A 工具入口整理说明，并保持原有 S02–S12 编号。
- [x] **Step 2: 激活 S01A 状态**，将 `current_status.md` 与 `PROJECT_CONTEXT.md` 更新为 S01A `IN_PROGRESS`、S01 为 Last Closed Stage、S02 为后续固件阶段；根 README 标明当前工作项为 S01A。
- [x] **Step 3: 增补项目上下文**，在 `PROJECT_CONTEXT.md` 加入 S01A 的 Required Reading，保留 S01 已验证工具链能力及未确认硬件事实。
- [x] **Step 4: 回读四份文件**，确认 S01A 当前状态、S01 关闭记录、S02 后续位置和路线图顺序互相一致，且不新增硬件事实。

**Verification:** 静态回读四份上下文文件；与 S01 handoff/review 及 S02 当前未冻结状态逐项对照。

**Output:** 一致的阶段路线和项目恢复入口。

### Task 4: 形成阶段证据并执行独立 Review

**Files:**

- Create: `00_Project/03_Stages/S01A_本机工具Skill接入与入口整理/handoff.md`
- Create: `00_Project/03_Stages/S01A_本机工具Skill接入与入口整理/review.md`
- Create: `04_Test/Reports/Stages/S01A/verification.md`
- Modify: `README.md`, `PROJECT_CONTEXT.md`, `00_Project/05_Status/current_status.md`

**Interfaces:**

- `handoff.md` 记录实施基线、改动文件、静态核对结论和未执行的操作。
- `verification.md` 区分项目文件静态核对、Skills 工具动作和真实硬件验证；后两者本阶段不执行。
- 独立 Reviewer 按设计、计划、变更和 verification evidence 给出 PASS / CHANGES_REQUESTED / BLOCKED；PASS 后将 S01A 置为 CLOSED。

- [x] **Step 1: 写 verification report**，记录 JSON/PowerShell/文档的静态核对证据；Keil Build、Flash、RTT、GDB 和硬件验证标记 `NOT_RUN`，不作通过声明。
- [x] **Step 2: 写 handoff**，列出配置模板、初始化脚本、README 和上下文更新；记录本机 `.embeddedskills` 文件保持未修改、S02 留作下一阶段。
- [x] **Step 3: 请求一个新 Reviewer 独立检查**设计覆盖、文件范围、初始化脚本不覆盖行为、敏感/本机参数边界、上下文一致性和验证结论。
- [x] **Step 4: 处理 Review 发现**，仅在设计范围内修正，并更新 verification、handoff 和 review 记录。
- [x] **Step 5: Review 通过后关闭阶段**，将 S01A 设为 `CLOSED`、更新 Last Closed Stage 和 handoff/review 状态，并将根 README、`PROJECT_CONTEXT.md`、`current_status.md` 的下一工作项同步到 S02；S02 的硬件/固件内容保持未冻结。

**Verification:** Reviewer 复核所有验收项；检查 stage status、handoff、review、verification、README、PROJECT_CONTEXT 和 current_status 一致。

**Output:** 可追溯的 S01A 验证、交接、Review 和关闭记录。

### Task 5: 提交并推送 S01A

**Files:**

- Stage files and project documents listed in Tasks 1–4.
- `00_Project/03_Stages/S01A_本机工具Skill接入与入口整理/design.md`
- `00_Project/03_Stages/S01A_本机工具Skill接入与入口整理/implementation_plan.md`

**Interfaces:**

- Commit only the reviewed S01A project changes.
- Push the completed commit on the current project branch to its configured upstream (`origin/main` at the approved plan baseline).

- [x] **Step 1: Review final diff**，确认只包含 Tasks 1–4 文件；排除 `.embeddedskills/`、`06_Output/` 和其他本机状态。
- [x] **Step 2: Run repository whitespace check** with `git diff --check` and confirm the working tree contains no unrelated user changes.
- [x] **Step 3: Commit the completed stage** with message `feat(tools): add S01A skill tooling entrypoints`; record the resulting implementation commit ID.
- [x] **Step 4: Record the implementation commit ID** in S01A `handoff.md` and `PROJECT_CONTEXT.md`.
- [x] **Step 5: Commit the handoff metadata** with message `docs(project): record S01A implementation handoff`.
- [x] **Step 6: Push both commits** to the current branch's configured upstream; confirm the pushed commits are the new branch tips without printing credential-bearing remote data.

**Verification:** `git status --short --branch` reports the intended branch synchronized with its upstream after push; the recorded implementation commit ID resolves in local Git history.

**Output:** S01A committed and pushed; S02 is the next work item.

## Verification summary

- Code verification: Static review of the initialization script and profile; no automated tests are added or run.
- Documentation verification: Cross-file status and path review, plus `git diff --check`.
- Hardware verification: `NOT_RUN` by design.
- Evidence location: `04_Test/Reports/Stages/S01A/verification.md`

## Execution gate

Do not start implementation until the Project Owner reviews and approves this written plan and selects the execution method. S01A begins only after the project status is updated to an implementation-ready state.
