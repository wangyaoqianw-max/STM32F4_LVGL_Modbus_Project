# 参考资料转换与校验状态

状态只表示文件是否存在、是否非空以及 Markdown 本地图片引用是否可达；不代表 MinerU 来源已确认，也不代表 OCR/版面识别准确性已经人工审校。

| 状态 | PDF 路径 | Markdown / 图片路径 | 页数 | 校验记录 |
|---|---|---|---:|---|
| `OUTPUTS_PRESENT` | `Datasheets/XPT2046/XPT2046_XPT_Datasheet_2007-05.pdf` | 同目录同名 `.md` 与 `images/` | 30 | PDF、MD、引用图片均非空；引用路径完整。识别准确性未审校。 |
| `OUTPUTS_PRESENT` | `Datasheets/STM32F407_DS8626/STM32F407xx_DS8626_Rev12-1.pdf` | 同目录同名 `.md` 与共享 `images/` | 150 | PDF、MD、引用图片均非空；分片与下一片合计覆盖原件 206 页。识别准确性未审校。 |
| `OUTPUTS_PRESENT` | `Datasheets/STM32F407_DS8626/STM32F407xx_DS8626_Rev12-2.pdf` | 同目录同名 `.md` 与共享 `images/` | 56 | PDF、MD、引用图片均非空；与上一片页数之和为 206。识别准确性未审校。 |
| `OUTPUTS_PRESENT` | `Datasheets/W25Q128/w25q128jv revf 03272018 plus.pdf` | 同目录同名 `.md` 与 `images/` | 78 | PDF、MD、引用图片均非空；引用路径完整。识别准确性未审校。 |
| `WAITING_MANUAL_CONVERSION` | `Reference_Manuals/STM32F4_RM0090_Rev22/STM32F4_RM0090_Rev22.pdf` | 46 个书签分片位于同目录 `RM0090/`；尚无 MD | 1741 | 分片连续覆盖 1–1741 页、无缺页，单片不超过 163 页。可逐片转换并将 Markdown/图片放在同版本目录。 |
| `WAITING_MANUAL_CONVERSION` | `Reference_Manuals/PM0214_Cortex-M4_Programming_Manual_parts/STM32F3与F4系列Cortex-M4内核编程手册1.pdf` | 同目录同名 `.md` | 199 | 来源工程仅提供 PDF；当前分片符合 199 页上限。 |
| `WAITING_MANUAL_CONVERSION` | `Reference_Manuals/PM0214_Cortex-M4_Programming_Manual_parts/STM32F3与F4系列Cortex-M4内核编程手册2.pdf` | 同目录同名 `.md` | 46 | 来源工程仅提供 PDF；单份转换即可。 |

`ILI9341/`、`MAX485_ADI/` 和 `Protocols/Modebus/` 含本机参考文件及转换稿，但由于再分发许可未确认或原文明确限制分发，整个目录被 `.gitignore` 排除，不纳入仓库转换清单。不要将其文件加入提交。本机 `ILI9341-1.md` 还发现一处 MinerU 识别异常（将 `240×RGB(H)` 识别成 Markdown 链接），内容待人工校正。
