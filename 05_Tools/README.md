# 项目工具

## 使用边界

常见嵌入式操作优先使用本机已安装的 Embedded Skills。本目录保存本工程共享配置示例、首次初始化入口和调用速查；不复制 Skill 包、工具程序或 Skill 通用脚本。只有出现已确认且重复的 Skill 缺口时，才为该具体操作增加项目脚本。

## 项目配置

- 配置示例：`05_Tools/Config/embeddedskills.config.example.json`。
- 首次初始化：`powershell -NoProfile -File .\05_Tools\Scripts\Initialize-EmbeddedSkillsConfig.ps1`。
- 脚本只在 `.embeddedskills/config.json` 不存在时从示例创建；目标文件已存在时提示并保留，不覆盖、不合并。`.embeddedskills/` 已由 Git 忽略。
- 示例中的工程路径相对仓库根目录。Keil/J-Link 可执行文件、GDB 与 J-Link GDB Server 路径及探针序列号由用户级 Skill 配置管理，不写入项目示例。
- `STM32F407VE / SWD / 1000` 是当前 J-Link Skill 的项目工具参数，不单独证明本次连接设备或实物板卡身份。

## 常用入口

| 工作 | Skill 调用 | 输入 / 说明 |
|---|---|---|
| 扫描 Keil 工程、枚举 Target | `/keil scan`、`/keil targets` | 工程文件与 Target 已在项目配置示例中登记。 |
| 构建 Keil 工程 | `/keil build` | 返回最近构建产物；烧录前由 `keil` Skill 确认构建成功。 |
| 探测 J-Link | `/jlink info` | 需要本次可用的探针、目标和连接参数。 |
| 烧录固件 | `/jlink flash` | 使用 Keil 返回的固件产物；只有当前阶段和用户要求允许时执行。 |
| 读取 RTT | `/jlink rtt` | 由 J-Link Probe 占用；同一 Probe 不与其他 RTT/GDB/J-Link 会话并行使用。 |
| GDB 源码级调试 | `/jlink gdb backtrace`、`/jlink gdb locals` | 使用 Keil 生成的 AXF（ELF）；GDB 与 GDB Server 路径来自用户级 Skill 配置。 |
| 明确要求多步骤编排 | `/workflow` | 仅在用户要求整条 Build/Flash/Debug/Observe 流程或自动诊断时使用；单项操作直接调用对应 Skill。 |

源码分析与固件修改分别使用 `embedded-code-reader` 和 `embedded-code-development`；串口监视使用 `serial`。`gcc`、`eide`、`openocd`、`probe-rs` 是可选后端，只有阶段明确选择后才配置；CAN Skill 仅在需求加入 CAN 后使用。

## 操作与产物边界

执行目标连接、烧录、复位、在线调试或串口操作前，需确认本次目标设备、探针、接口参数和阶段授权。S01 的历史验证不代表当前设备连接状态已确认；静态检查或工具命令成功也不等于真实硬件验证。

构建、调试和测试生成物及日志写入 `06_Output` 或对应测试输出目录；正式验证结论写入 `04_Test/Reports`。新增项目脚本前需明确其用途、输入输出、机器依赖、共享硬件资源和验证方式。
