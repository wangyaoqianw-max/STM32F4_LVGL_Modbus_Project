# Firmware

`03_Firmware` 是固件实现区。

当前状态：

- STM32F407VET6 Application 母工程已经建立；
- `S01 Application 工程初始化与诊断基础` 已 Review PASS 并关闭；
- 下一步进入 S02 板级基础能力 / Platform Bring-up 的设计讨论。

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

已建立：

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

架构保持：

```text
APP
 ↓
Service
 ↓
Platform
 ↓
Impl
 ↓
HAL / RTOS / Vendor
```

S01 已完成 Keil Build/Output 规范、五层基础框架、RTT、EasyLogger、Service Log、CmBacktrace、Fault Adapter 和基础 Build/Flash/Run 工具链验证。

`platform_types.h` 是冻结基础资源，按 Embedded Engineering Library S01 基线版本继续使用，不在后续阶段因风格偏好顺手修改。

S01 正式交接：

`00_Project/03_Stages/S01_Application工程初始化与诊断基础/handoff.md`
