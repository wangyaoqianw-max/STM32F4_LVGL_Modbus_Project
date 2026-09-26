# Firmware

`03_Firmware` 是固件实现区。

当前 Application 母工程已经建立，活动工作项为：

`S01 Application 工程初始化与诊断基础`

目录职责：

```text
03_Firmware/
├── 00_Doc/
├── Application/
├── Bootloader/
└── Shared/
```

- `Application`：STM32F407VET6 + FreeRTOS 主应用工程。
- `Bootloader`：独立启动、安装和恢复工程；在后续批准阶段启用。
- `Shared`：只保存 Application 与 Bootloader 两个真实使用方共同依赖且稳定的数据契约/代码。
- `00_Doc`：架构、接口、编码规则、Keil 构建规范和诊断迁移规则。

当前 Application：

`03_Firmware/Application/STM32F407_APP/`

S01 将在不移动 CubeMX 生成目录的前提下建立：

```text
00_Config/
01_APP/
02_Service/
03_Platform/
04_Impl/
05_Vendors/
Core/
Drivers/
Middlewares/
MDK-ARM/
```

五层架构与 Diagnostics 优先复用 `Embedded_Engineering_Library` 中已有成熟资产。

特别约束：Library 的 `platform_types.h` 作为项目基础资源直接使用，不在 S01 修改或重构。

阶段设计与施工计划见：

`00_Project/03_Stages/S01_Application工程初始化与诊断基础/`
