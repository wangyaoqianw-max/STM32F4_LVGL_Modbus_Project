# 参考资料索引

更新日期：2026-09-26

本索引按当前工程已确认基线记录适用性。来源工程的板级接线、显示模块规格和芯片型号不自动视为本项目硬件事实。器件后缀未确认时，资料仅作系列参考。

## 已从前两个项目复制

| 资料 | 版本 / 来源 | 本地路径 | 适用性与状态 |
|---|---|---|---|
| DHT20 产品规格书 | ASAIR，V1.3，2024-09 | `Datasheets/DHT20_ASAIR_V1.3_2024-09/`（PDF、MD、图片） | 与 DHT20 对应；未复制源工程专用的软件 I²C 设计分析。 [厂商产品页](https://www.aosong.com/en/Products/info.aspx?itemid=2317) |
| AT24C02 / 24Cxx 规格书 | ICHIP 英锐芯；文档未见版本号 | `Datasheets/AT24C02_IDCHIP_reference/`（PDF、MD、图片） | 系列参考；目标芯片供应商和后缀待核对。 [厂商产品目录](https://www.idchip.cn/a/product/) |
| HC-05 蓝牙串口模块资料 | ITead Studio，2010-06-18 | `Datasheets/HC-05_ITead_2010-06-18/`（PDF、MD、图片） | 通用 HC-05 模块参考；本项目具体载板和固件版本待核对。来源为 `STM32F4_Bootloader_OTA_Test_Project/01_Reference/Datasheets/bluetooth-hc05-datasheet/`。 |
| W25Q64JVSSIQ 串行 Flash 规格书 | Winbond，Rev J，2018-03-27 | `Datasheets/W25Q64JVSSIQ_reference_only/`（PDF、MD、图片） | **仅供近似系列参考**。工程基线为 W25Q128，不能用此资料确认目标 Flash 的容量、后缀或封装。 [Winbond 文档页](https://www.winbond.com/hq/support/documentation/?__locale=en&category=%2F.categories%2Fresources%2Fdatasheet%2F&family=%2Fproduct%2Fcode-storage-flash-memory%2Fserial-nor-flash%2Findex.html&line=%2Fproduct%2Fcode-storage-flash-memory%2Findex.html&pno=W25Q64JV) |
| Cortex-M4 编程手册（分卷） | 来源工程中的版本未核实 | `Reference_Manuals/PM0214_Cortex-M4_Programming_Manual_parts/`（2 个 PDF；源目录没有 MD） | Cortex-M4 内核补充参考；保留原始 PDF。 |

复制时保留了 Markdown 引用的 `images/` 附件；源工程文件未移动或修改。

## 已下载并纳入仓库

| 资料 | 版本 / 来源 | 本地路径 | 适用性与状态 |
|---|---|---|---|
| STM32F405xx / STM32F407xx 数据手册 | ST DS8626 Rev 12，2026-03，206 页 | `Datasheets/STM32F407_DS8626/`（原 PDF；150 页与 56 页分片 PDF；对应 MD、图片） | ST 原始 PDF，适用于 STM32F407VET6。分片合计 206 页；图片引用路径已检查，未验证 Markdown 识别准确性。`official:true`。 [ST PDF](https://www.st.com/resource/en/datasheet/stm32f407ve.pdf) |
| STM32F4 参考手册 | ST RM0090 Rev 22，2026-05，1741 页 | `Reference_Manuals/STM32F4_RM0090_Rev22/STM32F4_RM0090_Rev22.pdf`；同目录 `RM0090/` 内 46 个书签分片 | ST 原始 PDF；原文件已从旧路径迁入版本目录，文件内容一致。分片覆盖 1–1741 页且无缺页，单片最多 163 页；尚无 Markdown。`official:true`。 [ST PDF](https://www.st.com/resource/en/reference_manual/dm00031020-stm32f4xxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) |
| XPT2046 触摸控制器数据手册 | XPT 原始文档，2007-05；由 JLCPCB 产品页提供 | `Datasheets/XPT2046/`（PDF、MD、图片） | **`official:false`**。30 页，PDF 和 Markdown/图片均存在，图片路径已检查；未验证识别准确性。 [分销商来源](https://jlcpcb.com/partdetail/XPT2046/C19076) · [PDF](https://jlcpcb.com/api/file/downloadByFileSystemAccessId/8588881458337894400) |
| W25Q128JV 串行 Flash 规格书 | Winbond，Rev F，2018-03-27，78 页 | `Datasheets/W25Q128/`（PDF、MD、图片） | 官方系列参考；工程只确认 W25Q128 容量，具体完整料号、封装和温度等级仍待核对。Markdown 图片路径已检查，未验证识别准确性。`official:true`。 [Winbond 资料页](https://www.winbond.com/hq/support/documentation/?__locale=en&category=%2F.categories%2Fresources%2Fdatasheet%2F&family=%2Fproduct%2Fcode-storage-flash-memory%2Fserial-nor-flash%2Findex.html&line=%2Fproduct%2Fcode-storage-flash-memory%2Findex.html&pno=W25Q128JV) |

## 本机保留但不纳入仓库

以下资料的 PDF、分片、Markdown 和图片均由 `.gitignore` 排除在提交之外。相关文件可在本机继续查阅；版权或再分发许可没有确认时不推送。

| 资料 | 本机路径 | 本机状态与原因 |
|---|---|---|
| ILI9341 LCD 控制器资料 | `Datasheets/ILI9341/` | eeworld 镜像提供的 ILITEK V1.02 Preliminary，原件 233 页；另有 199 页和 34 页分片及 Markdown/图片。第 2 页载有未经书面许可不得分发或复制的声明，因此整目录不推送。 [来源页](https://datasheet.eeworld.com.cn/view/57286407.html) |
| MAX485 收发器数据手册 | `Datasheets/MAX485_ADI/` | Analog Devices MAX1487–MAX491 Rev 11，17 页，覆盖原理图标注的 MAX485；本机有 PDF 和 Markdown。再分发许可未明确，整目录不推送。 [ADI 产品页](https://www.analog.com/en/products/max485.html) · [官方 PDF](https://www.analog.com/media/en/technical-documentation/data-sheets/MAX1487-MAX491.pdf) |
| Modbus Serial Line Protocol and Implementation Guide | `Protocols/Modebus/Modbus_Serial_Line_V1.02.pdf`、同名 `.md` | Modbus Organization V1.02，44 页；本机 PDF/Markdown 可用。再分发许可未明确，整目录不推送。 [官方规范目录](https://www.modbus.org/modbus-specifications) · [官方 PDF](https://www.modbus.org/file/secure/modbusoverserial.pdf) |
| Modbus Application Protocol Specification | `Protocols/Modebus/Modbus_Application_Protocol_V1.1b3.pdf`、同名 `.md` | Modbus Organization V1.1b3，50 页；本机 PDF/Markdown 可用。再分发许可未明确，整目录不推送。 [官方规范目录](https://www.modbus.org/modbus-specifications) · [官方 PDF](https://www.modbus.org/file/secure/modbusprotocolspecification.pdf) |

## 当前待确认

- W25Q128 的完整料号、封装与温度等级仍需与实物丝印或 BOM 核实；W25Q128JV Rev F 目前只作系列参考。
- 显示模块原理图和尺寸图、STM32F407VE 开发板原理图和尺寸图，以及 ILI9341/XPT2046 模块硬件事实分析保存在 `02_Hardware/Hardware_Software_Interface/`。具体接线与屏幕总线配置应以这些板级材料核实，不能仅由控制器手册推定。
- `P169H002-CTP / ST7789T3` 屏幕资料对应 240×280 SPI 电容触摸模组；当前工程基线是 ILI9341 并口/FMC 与 XPT2046 电阻触摸，不作为本项目器件资料复制。
- STM32F411 芯片手册和板级设计资料属于来源工程器件，不替代 STM32F407 手册或本项目硬件事实。
- TI《The RS-485 Design Guide》SLLA272D Rev D 仅登记来源链接：[TI PDF](https://www.ti.com/lit/an/slla272d/slla272d.pdf)。其使用条款将许可限定为开发含 TI 产品的应用并禁止其他复制/展示；图中收发器为 ADI MAX485，因此未保留 PDF。
