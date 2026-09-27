# Application 工程

本目录用于 STM32F407VET6 FreeRTOS Application。

当前实际工程：

`STM32F407_APP/`

## 当前状态

`S01 Application 工程初始化与诊断基础` 已经完成并 Review `PASS`。

当前 Application 已具备：

- CubeMX/HAL + FreeRTOS 母工程；
- SWD、SPI1、USART1/2/3、UART DMA/IRQ、FSMC、XPT2046 GPIO、软件 I2C 的静态配置基线；
- 规范化 Keil Build 输出；
- 五层基础架构；
- J-Link Build / Flash / Run 基础链；
- RTT + EasyLogger + Service Log；
- CmBacktrace + Fault Adapter；
- CubeMX Generate 回归检查基线。

## 架构

```text
APP
 ↓
Service
 ↓
Platform
 ↓
Impl
 ↓
HAL / RTOS / Vendor / Hardware
```

CubeMX 生成的 `Core/`、`Drivers/`、`Middlewares/` 不移动。

优先从 `wangyaoqianw-max/Embedded_Engineering_Library` 复用已有资产，不再从旧 F411 工程复制已经沉淀到 Library 的实现。

`platform_types.h` 是要求保留的基础资源，当前与 S01 冻结 Library Blob 一致，后续继续原样使用。

## 下一步

下一工作项是 S02 板级基础能力 / Platform Bring-up。S02 尚未冻结设计，进入代码施工前先重新确认具体 Bring-up 范围、复用资产、测试节点和验收条件。

S01 交接与 Review：

- `00_Project/03_Stages/S01_Application工程初始化与诊断基础/handoff.md`
- `00_Project/03_Stages/S01_Application工程初始化与诊断基础/review.md`
