# Firmware Agent Instructions

修改 `03_Firmware` 下文件前：

1. 阅读仓库根 `AGENTS.md`、`PROJECT_CONTEXT.md` 和当前阶段设计/计划/交接。
2. 阅读本目录 `README.md`、`00_Doc/README.md` 和目标子目录的工程说明。
3. 对照 `02_Hardware` 和 `01_Reference` 核验引脚、外设、DMA、IRQ、器件型号和电气约束。
4. 只修改当前批准计划列出的范围；发现计划或硬件事实冲突时先记录并暂停相关改动。

不得直接复制来源工程的启动文件、linker、时钟、引脚、DMA 映射或外设句柄。Application 与 Bootloader 保持独立工程边界；只有经两个实际使用方确认的稳定模块才进入 `Shared`。
