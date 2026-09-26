# 参考资料索引

更新日期：2026-09-26

本索引按当前工程已确认基线记录适用性。来源工程的板级接线、显示模块规格和芯片型号不自动视为本项目硬件事实。

## 已从前两个项目复制

| 资料 | 版本 / 来源 | 本地路径 | 适用性与状态 |
|---|---|---|---|
| DHT20 产品规格书 | ASAIR，V1.3，2024-09 | `Datasheets/DHT20_ASAIR_V1.3_2024-09/`（PDF、MD、图片） | DHT20 对应资料，已复制；未复制源工程专用的软件 I²C 设计分析。 [厂商产品页](https://www.aosong.com/en/Products/info.aspx?itemid=2317) |
| AT24C02 / 24Cxx 规格书 | ICHIP 英锐芯；文档未见版本号 | `Datasheets/AT24C02_IDCHIP_reference/`（PDF、MD、图片） | 系列参考，目标芯片供应商和后缀待核对。 [厂商产品目录](https://www.idchip.cn/a/product/) |
| HC-05 蓝牙串口模块资料 | ITead Studio，2010-06-18 | `Datasheets/HC-05_ITead_2010-06-18/`（PDF、MD、图片） | 通用 HC-05 模块参考；本项目具体载板和固件版本待核对。来源为 `STM32F4_Bootloader_OTA_Test_Project/01_Reference/Datasheets/bluetooth-hc05-datasheet/`。 |
| W25Q64JVSSIQ 串行 Flash 规格书 | Winbond，Rev J，2018-03-27 | `Datasheets/W25Q64JVSSIQ_reference_only/`（PDF、MD、图片） | **仅供近似系列参考**。当前工程基线为 W25Q128，不能据此确认目标 Flash 的容量、后缀或封装。 [Winbond 文档页](https://www.winbond.com/hq/support/documentation/?__locale=en&category=%2F.categories%2Fresources%2Fdatasheet%2F&family=%2Fproduct%2Fcode-storage-flash-memory%2Fserial-nor-flash%2Findex.html&line=%2Fproduct%2Fcode-storage-flash-memory%2Findex.html&pno=W25Q64JV) |
| Cortex-M4 编程手册（分卷） | 来源工程中版本未核实 | `Reference_Manuals/PM0214_Cortex-M4_Programming_Manual_parts/`（2 个 PDF；源目录没有 MD） | Cortex-M4 内核补充参考；已复制 PDF 原件。 |

复制时保留了 Markdown 引用的 `images/` 附件；源工程文件未移动或修改。

## 已下载

| 资料 | 版本 / 来源 | 本地路径 | 适用性与状态 |
|---|---|---|---|
| STM32F405xx / STM32F407xx 数据手册 | ST DS8626 Rev 12，2026-03，206 页 | `Datasheets/STM32F407_DS8626/STM32F407xx_DS8626_Rev12.pdf` | ST 原始 PDF；已核对封面、页数与 PDF 文件头，适用于 STM32F407VET6。 `official:true`。 [ST PDF](https://www.st.com/resource/en/datasheet/stm32f407ve.pdf) |
| STM32F4 参考手册 | ST RM0090 Rev 22，2026-05，1741 页 | `Reference_Manuals/STM32F4_RM0090_Rev22.pdf` | ST 原始 PDF；已核对封面、页数与 PDF 文件头，覆盖 STM32F407。 `official:true`。 [ST PDF](https://www.st.com/resource/en/reference_manual/dm00031020-stm32f4xxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) |
| XPT2046 触摸控制器数据手册 | XPT 原始文档，2007-05；由 JLCPCB 产品页提供 | `Datasheets/XPT2046/XPT2046_XPT_Datasheet_2007-05.pdf` | **`official:false`**。器件与当前电阻触摸基线相符；PDF 原件已核验非空，等待人工 MinerU 转换。 [分销商来源](https://jlcpcb.com/partdetail/XPT2046/C19076) · [PDF](https://jlcpcb.com/api/file/downloadByFileSystemAccessId/8588881458337894400) |

## 待获取或待确认

| 资料 | 官方版本 / 来源 | 预期路径 | 状态 |
|---|---|---|---|
| ILI9341 LCD 控制器资料 | ILITEK 原始文档 V1.02 Preliminary，233 页；eeworld 镜像 | 本机 `Datasheets/ILI9341/ILI9341.pdf`（仅本地） | 已核对封面与页数。第 2 页声明未经书面许可不得分发或复制；文件保留在本机但不纳入 Git，也不安排仓库内转换。 [候选页面](https://datasheet.eeworld.com.cn/view/57286407.html) |
| W25Q128 数据手册 | Winbond 官方资料页列有 W25Q128JV 与 W25Q128JV_DTR | `Datasheets/W25Q128/` | 当前工程只确认 W25Q128 容量，具体后缀/器件仍待确认；暂不选型下载。 [Winbond 资料页](https://www.winbond.com/hq/support/documentation/?__locale=en&category=%2F.categories%2Fresources%2Fdatasheet%2F&family=%2Fproduct%2Fcode-storage-flash-memory%2Fserial-nor-flash%2Findex.html&line=%2Fproduct%2Fcode-storage-flash-memory%2Findex.html&pno=W25Q128JV) |

## 未复制的源工程资料

- `P169H002-CTP / ST7789T3` 屏幕资料对应 240×280 SPI 电容触摸模组；当前工程基线是 ILI9341 并口/FMC 与 XPT2046 电阻触摸，故不作为本项目器件资料复制。
- STM32F411 芯片手册和板级设计资料属于源工程器件，不替代 STM32F407 手册或本项目硬件事实。
