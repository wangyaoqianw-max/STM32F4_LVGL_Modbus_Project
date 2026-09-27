# 04_Impl 实现层

本层承载 Platform API 的目标适配。`impl_os/freertos/` 使用当前工程提供的 CMSIS-RTOS2 接口；`impl_board/board_types.h` 是冻结 `platform_types.h` 的直接类型依赖；`diagnostics/` 提供项目自己的 CmBacktrace Port 和 Fault Adapter。
