# Current Status

## 当前状态

- Active Stage: `S03 W25Q128 与中文字库基础`
- Active Stage Status: `DRAFT`
- Last Closed Stage: `S02 板级基础能力 / Platform Bring-up`
- Last Closed Stage Status: `CLOSED`
- Owner: `Project Owner`
- Branch: `main`
- S01 Implementation Commit: `eabc22ed58e770a80f41dcd94ce427710a658c44`
- S01A Implementation Commit: `7a942b57d75e2c1c2c5c0bd2238141b12898cf0a`
- S02 Design Baseline Commit: `0ffe747eeb0a1e7e2c6a088e2b402bbc57965f44`
- S02 Implementation Commit: `87762406c602cbe4b896203c833e640b1371ff0c`
- S02 Review Result: `PASS`
- S01 Stage Docs: `00_Project/03_Stages/S01_Application工程初始化与诊断基础/`
- S01A Stage Docs: `00_Project/03_Stages/S01A_本机工具Skill接入与入口整理/`
- S02 Stage Docs: `00_Project/03_Stages/S02_板级基础能力与Platform_Bring-up/`
- Next Work Item: `S03 Design / Implementation Plan`
- Next Action: S02 已通过独立 Review 并关闭；启动 S03 设计与实施计划，设计批准前不进入施工。

## 已形成的 Application 基线

- STM32F407VET6 CubeMX/HAL + FreeRTOS 母工程已建立。
- 基础 Build / J-Link Flash / Reset / Run 工具链已经跑通。
- Keil Build Artifact 已规范到 `MDK-ARM/Objects/` 和 `MDK-ARM/Listings/`。
- 五层基础框架已建立：`APP → Service → Platform → Impl → HAL/RTOS/Vendor`。
- Embedded Engineering Library Commit `8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0` 继续作为当前冻结复用来源。
- `platform_types.h` 作为基础资源保持冻结 Library Blob，不修改。
- RTT、EasyLogger、Service Log、CmBacktrace、Fault Adapter 已接入并通过 S01 验证。
- CubeMX Generate 回归已实际执行一次，Generate 后 Clean Rebuild `PASS`。
- S01 Review：`PASS`，阶段正式关闭。
- S01A 工具入口整理已关闭，未改变固件架构和硬件事实。

## S02 已冻结设计边界

S02 的目标不是完成所有外设 Driver，而是完成：

```text
CubeMX / HAL Resource
        ↓
STM32F407 Impl
        ↓
Board / HAL Binding
        ↓
Platform API
```

本阶段允许迁入：

- GPIO；
- Delay；
- Software I2C；
- SPI；
- UART；
- IRQ；
- Reset；
- 对应 STM32F4 Impl Backend。

明确暂缓：

- Watchdog；
- W25Q128 完整 Driver；
- AT24C02 完整 Driver；
- DHT20；
- UART DMA + IDLE + RingBuffer Runtime；
- UART Service；
- ILI9341 / XPT2046 / LVGL；
- RS485 / Modbus；
- OTA / YMODEM；
- Bootloader。

架构决策：

- 不新增通用 `platform_dma`。
- 不新增通用 `platform_fmc`。
- SPI 真实设备验证留 S03，通过 W25Q128 JEDEC/Read 完成。
- UART DMA Runtime 留 S04。
- FMC/LCD 实机时序留 S06。
- Reset 可以迁入，但不单独安排人工板测。
- 不建立依赖未来全部设备存在的完整初始化总入口。

## S02 最小验证目标

- Clean Rebuild。
- GPIO 最小 Smoke Test。
- PB6/PB7 Software I2C 对板载 AT24C02 做地址 ACK Probe；不写 EEPROM。
- 从 USART1/2/3 中选择最方便的一路完成 Blocking TX/RX Smoke。
- SPI、IRQ/DMA、FMC 仅执行本阶段需要的 Binding / Static / Build 检查，未执行的设备功能必须记录为 `DEFERRED`。

详细设计和执行计划：

- `00_Project/03_Stages/S02_板级基础能力与Platform_Bring-up/design.md`
- `00_Project/03_Stages/S02_板级基础能力与Platform_Bring-up/implementation_plan.md`

## Carry-forward / TO_VERIFY

以下内容不是当前阶段失败项，继续带入相关后续阶段：

- HSE 实际晶振频率；
- 实物板卡身份的独立确认；
- PB6/PB7 外部上拉；
- HC-05 实际 UART 参数；
- Modbus 正式 UART 参数；
- RS485 收发方向控制；
- LCD/FSMC 实际时序；
- 当前 Pinout 文档中的 PLL48 描述与实际未使用 PLL48 域配置的一致性；
- J-Link 历史间歇连接失败原因，仅在复现时继续诊断。

## 当前执行门

S02 已按批准的 Implementation Plan 完成实施与计划内验证，独立 Review 结论为 `PASS`，阶段已关闭。当前活动工作项切换至 `S03 W25Q128 与中文字库基础`，状态为 `DRAFT`；下一步先完成 S03 Design / Implementation Plan 并按流程批准后再施工。
