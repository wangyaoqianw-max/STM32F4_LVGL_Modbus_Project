# Firmware

`03_Firmware` 是固件实现区。当前尚未导入或生成固件工程；正式工程创建应按阶段计划执行。

```text
03_Firmware/
├── 00_Doc/
├── Application/
├── Bootloader/
└── Shared/
```

- `Application`：FreeRTOS 主应用及业务服务。
- `Bootloader`：独立启动、安装和恢复工程；按批准的阶段启用。
- `Shared`：仅放置由两个真实使用方共同依赖的稳定代码。
- `00_Doc`：架构、接口、编码规则和内存/资源约束。

层次、依赖方向和模块职责以工程设计决策及当前阶段设计为准。
