# Application 工程

本目录用于 STM32F407VET6 FreeRTOS Application。

当前实际工程：

`STM32F407_APP/`

当前状态：

- CubeMX/HAL + FreeRTOS 母工程已经生成；
- 主要 Pinout、FSMC、SPI1、USART1/2/3、DMA/IRQ、软件 I2C 和 SWD 静态配置已经进入 `.ioc`；
- 当前进入 S01，开始整理 Keil Build、五层架构基础框架和 Diagnostics。

S01 架构保持：

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

优先从 `wangyaoqianw-max/Embedded_Engineering_Library` 复用已有资产，不再从旧 F411 工程直接复制。

`platform_types.h` 是要求保留的基础资源，迁入后按 Library 当前版本原样使用，不在 S01 修改。

当前阶段设计：

`00_Project/03_Stages/S01_Application工程初始化与诊断基础/`
