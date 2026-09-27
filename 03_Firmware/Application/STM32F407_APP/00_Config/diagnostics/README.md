# 诊断配置

- `cmb_user_cfg.h` 通过 Keil C 编译宏 `CMB_USER_CFG` 供 CmBacktrace 读取，诊断输出直接写入 RTT Channel 0。
- `diagnostics_config.h` 保存 Firmware、MCU 和阶段版本字符串。`STM32F407VET6` 表示软件目标 MCU，不表示未经确认的实物板卡型号；`S01-dev` 是本阶段开发标识。
- `DIAG_FAULT_TEST_ENABLE` 默认必须为 `0U`。受控 Fault 验证结束后恢复该值并重建正常镜像。
- `freertos_tasks_c_additions.h` 由当前 FreeRTOS `tasks.c` 的扩展点包含，读取当前 TCB 提供 CmBacktrace 所需的任务名和栈边界。`configRECORD_STACK_HIGH_ADDRESS=1` 会在每个 TCB 中保留栈顶指针；此项不改变任务栈、FreeRTOS Heap 或 Main Stack 的配置尺寸。当前工程没有修改 `tasks.c`。
