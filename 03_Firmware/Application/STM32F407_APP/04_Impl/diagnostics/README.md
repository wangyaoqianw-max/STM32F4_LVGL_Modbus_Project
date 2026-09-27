# Diagnostics 实现

- `cmbacktrace_port.c` 从 `00_Config/diagnostics/diagnostics_config.h` 读取 Firmware / MCU / 版本字符串。
- `diagnostics_fault.c` 先将异常上下文经 RTT 原始输出，再调用 CmBacktrace；Fault 路径不依赖 EasyLogger 或 RTOS 日志任务。
- `cmbacktrace_fault_handlers.S` 是 HardFault、MemManage、BusFault、UsageFault 的唯一强定义 Owner。CubeMX 启动文件中的弱回退保留；`stm32f4xx_it.c` 中同名 C 定义不参与构建。
- `05_Vendors/CmBacktrace/fault_handler/keil/cmb_fault.S` 保留原样，但不加入 Keil 工程。
- Fault Trigger 代码由 `DIAG_FAULT_TEST_ENABLE` 编译门控，默认关闭。
