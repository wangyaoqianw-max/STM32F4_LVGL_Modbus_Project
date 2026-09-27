# S01A Design

## Metadata

- Stage: `S01A 本机工具 Skill 接入与入口整理`
- Status: `DESIGN_APPROVED`
- Owner: `Project Owner`
- Date: 2026-09-27
- Design Baseline Commit: `c27f8af01be6e4910247a42078c7ec255fa15c98`

## Goal

在 `05_Tools` 保存本工程可复用的 Embedded Skills 项目配置示例、初始化入口和调用速查，方便后续直接使用 Keil、J-Link、GDB 等已安装工具能力。

Keil、J-Link、GDB 和工作流由现有 Skills 执行；本阶段只增加本工程需要的配置与薄入口，不复制 Skill 包、工具程序或其通用脚本。

## Inputs and constraints

- S01 已于 2026-09-27 关闭，Implementation Commit 为 `eabc22ed58e770a80f41dcd94ce427710a658c44`。
- 当前下一工作项是尚未冻结设计的 S02 `板级基础能力 / Platform Bring-up`。
- S01 已确认 Keil 工程路径为 `03_Firmware/Application/STM32F407_APP/MDK-ARM/STM32F407_APP.uvprojx`，Target 为 `STM32F407_APP`。
- 当前 `.embeddedskills/` 被 Git 忽略；其中已有本机配置和运行状态。本阶段不得覆盖或提交这些现有本机文件。
- 当前 J-Link skill 工程参数记录为 `STM32F407VE / SWD / 1000 kHz`。这是工具目标配置，不单独构成实物板卡身份确认。
- 仓库可提交文件不得包含本机可执行文件路径、探针序列号、凭据或仅对单一机器有效的状态。
- 依照仓库规则，本阶段不构建、烧录、复位或在线调试目标板。

## Scope

### In scope

1. 新增 `05_Tools/Config/embeddedskills.config.example.json`，保存可跨工作区复用的项目级 Keil 与 J-Link 参数，路径均相对仓库根目录。
2. 新增 `05_Tools/Scripts/Initialize-EmbeddedSkillsConfig.ps1`：从项目示例创建被忽略的 `.embeddedskills/config.json`；目标文件已存在时提示并退出，不覆盖、不合并。
3. 更新 `05_Tools/README.md`，明确项目配置与用户级 Skill 工具路径的边界，给出 Keil、J-Link、GDB 的调用速查；将过时的“当前无开发板”和“暂不创建项目 Skill 配置”说明改成适用于当前阶段的条件与入口说明。
4. 将 GDB 源码级调试映射到 `jlink` Skill 的 `gdb` 子命令；将跨步骤编排映射到 `workflow` Skill，并说明其只在明确要求整条工作流时使用。
5. 在 S01 与 S02 之间新增本支撑阶段，不改变 S01 已关闭的记录，也不提前冻结 S02 的硬件 Bring-up 设计。
6. 更新 `00_Project/02_Roadmap/development_roadmap.md`、`PROJECT_CONTEXT.md`、`00_Project/05_Status/current_status.md` 和根 `README.md`，将 S01A 排在 S01 之后、S02 之前；S01 保持 `CLOSED`，S02 保持后续固件阶段且不重新编号。

### Out of scope

- 复制或镜像 Skills 安装目录中的脚本、第三方工具程序或 Keil/J-Link/GDB 可执行文件。
- 为 Build、Flash、RTT、GDB 再实现项目包装器；这些动作继续由现有 Skills 提供。
- 修改本机 `.embeddedskills/config.json` / `state.json`，或用户级 Skill 配置中的安装路径。
- 配置尚未选定的 GCC、EIDE、OpenOCD 或 probe-rs 后端。
- 构建、清理、烧录、复位、在线调试、串口发送或其他板级操作。
- S02 Platform Bring-up 的固件功能和硬件验收。
- 修改 S02 的范围、硬件分配或验收设计。

## Confirmed facts, assumptions, and unknowns

### Confirmed

- S01 已关闭，`PROJECT_CONTEXT.md` 和 `current_status.md` 指向 S02 设计为下一正式工作项。
- `00_Project/02_Roadmap/development_roadmap.md` 将 S02 列为 S01 后的下一阶段；该路线图明确允许在进入具体阶段前调整阶段拆分和顺序。
- 本工程以 Keil MDK 为当前构建路径，以 J-Link 为当前主要调试/烧录路径。
- 已安装的 `jlink` Skill 包含 GDB 源码级调试子命令，并读取用户级配置中的 J-Link GDB Server 与 `arm-none-eabi-gdb` 路径。
- `.embeddedskills/config.json` 是项目级 Skill 配置；当前目录被 Git 忽略。本机现有文件包含 probe-rs 选择信息，不应复制进通用示例。
- `05_Tools/README.md` 已规定优先使用已安装 Skills，不复制其工具脚本。

### Assumptions / design choices

- S01A 作为一个短支撑阶段，安排在 S01 之后、S02 固件施工之前。
- 示例配置的 Keil 构建日志目录使用 `06_Output/ToolSkills/BuildLogs`，避免沿用 S01 的一次性任务目录。
- 示例保留项目级参数；Skill 可执行文件、GDB 路径、探针序列号和可选 Probe 选择继续保存在各自本机配置中。
- 初始化脚本仅执行首次创建，不自动修改或合并已有本机配置，减少覆盖其他 Skill 设置的风险。

### Unknown / To Verify

- S02 施工期间是否会出现当前 Skills 未覆盖、确实需要新增本工程专用脚本的操作；S01A 不为未出现的需求预建脚本框架。
- 实物板卡身份仍需由独立硬件证据确认；示例中的 `STM32F407VE` 只表示当前 J-Link 工具目标配置。

## Design and module boundaries

```text
05_Tools/Config/embeddedskills.config.example.json
                  │
                  ▼
05_Tools/Scripts/Initialize-EmbeddedSkillsConfig.ps1
                  │ 仅当目标文件不存在时复制
                  ▼
       .embeddedskills/config.json (ignored, local)
                  │
          ┌───────┴────────┐
          ▼                ▼
      Keil Skill       J-Link Skill
                           └── GDB 子命令
```

- `Config/embeddedskills.config.example.json` 只包含相对工程路径、Keil Target、项目日志目录和 J-Link 目标连接参数。
- 初始化脚本只负责定位仓库根目录、检查目标配置是否存在和首次复制；不安装 Skills、不读取或写入用户级工具路径，也不启动工具进程。
- `05_Tools/README.md` 是入口选择说明：Keil 工程扫描/构建使用 `keil` Skill；探针、Flash、RTT 和 GDB 使用 `jlink` Skill；整条 Build/Flash/Observe 编排按用户明确要求使用 `workflow` Skill。
- GDB 使用 Keil 产出的 AXF 作为符号文件；GDB 可执行文件和 J-Link GDB Server 路径归用户级 Skill 配置所有。
- 若以后确有 Skill 未覆盖的项目操作，再按单一用途增加 `05_Tools/Scripts` 脚本，并在对应阶段明确其输入、输出、机器依赖和验证方法。

## Resource ownership and interactions

- Skill 包与工具安装归本机环境管理；项目仓库不维护它们的副本。
- 项目级默认值由已跟踪的配置示例管理；每台机器的实际配置由 `.embeddedskills/config.json` 管理并保持忽略。
- 用户级 `keil` / `jlink` Skill 配置管理工具安装路径与操作模式，不写入本仓库。
- J-Link Probe 是共享硬件资源；RTT、GDB Server 和其他 J-Link 客户端不可并发占用同一 Probe。
- Build/调试日志写入 `06_Output`，正式阶段结论写入 `04_Test/Reports`。

## Failure behavior and recovery

- 若 `.embeddedskills/config.json` 已存在，初始化脚本不改动文件，提示用户保留现有配置或自行审阅后手动合并示例。
- 若 Skill 所需本机工具路径无效，由对应 Skill 按其配置规则报告；项目初始化脚本不猜测或搜索安装路径。
- 若 J-Link 目标身份、接口或速度需要调整，只更新本机项目配置并保留实际证据；不通过默认值宣称实物板卡身份。
- 若调用入口不适用于当前工程或 Skill 未安装，停止该工具动作并记录缺失项，不回退到复制 Skill 源码。

## Acceptance criteria

- [ ] 配置示例仅包含本工程可共享的参数，使用仓库相对路径，不含机器绝对路径、Probe 序列号或凭据。
- [ ] 首次运行初始化脚本会创建 `.embeddedskills/config.json`；目标文件已存在时不会覆盖或合并。
- [ ] README 提供 Keil、J-Link、GDB 的 Skill 选择与调用方式，并区分单项操作和整条 Workflow。
- [ ] README 不再把已完成的 S00 配置条件或“当前无开发板”写成当前状态；硬件操作边界改为条件式说明。
- [ ] 路线图、项目上下文和当前状态将 S01A 排在已关闭的 S01 与待设计的 S02 之间；S01 记录保持关闭，S02 的硬件事实和设计不变。
- [ ] 本阶段不包含 Skills 本体副本，也不新增 Build/Flash/GDB 的重复实现。
- [ ] `.embeddedskills/` 继续被 Git 忽略，日志仍落在 `06_Output`。
- [ ] 没有构建、烧录、复位、在线调试或串口发送操作；S01 记录保持关闭，S02 硬件事实不被改写。

## Risks and deferred items

- Skills 的命令接口和配置 schema 由本机安装版本提供；升级 Skill 后需核对 README 中的入口说明。
- 配置示例可能落后于本机实际工具版本；项目配置初始化只提供已确认的工程默认值，不保证安装环境或硬件连接可用。
- `.embeddedskills/config.json` 已存在时不自动合并，首次设置后的自定义值由用户自行保留。
- 若未来出现稳定且重复的 Skill 缺口，再单独评审新增本工程专用脚本，不在 S01A 建立通用 Adapter/Workflow 框架。

## Required reading

- `README.md`
- `PROJECT_CONTEXT.md`
- `00_Project/WORKFLOW.md`
- `00_Project/05_Status/current_status.md`
- `00_Project/03_Stages/S01_Application工程初始化与诊断基础/handoff.md`
- `00_Project/03_Stages/S01_Application工程初始化与诊断基础/review.md`
- `04_Test/Reports/Stages/S01/verification.md`
- `05_Tools/README.md`
- Installed `keil`, `jlink`, and `workflow` Skill instructions
