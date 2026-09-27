# 04_Impl 实现层

本层承载 Platform API 的目标适配。`impl_mcu/stm32f4/` 提供复用的 STM32F4 后端；`impl_board/board_platform_bindings.*` 将 PB6/PB7、PC5、CubeMX SPI1 和 USART1/2/3 资源构造成 Platform 对象，不负责调用 HAL 初始化。GPIO 时钟、DMA 和 NVIC 初始化仍由 CubeMX/HAL 路径管理。

`impl_os/freertos/` 使用当前工程提供的 CMSIS-RTOS2 接口；`impl_board/board_types.h` 是冻结 `platform_types.h` 的直接类型依赖；`diagnostics/` 提供项目自己的 CmBacktrace Port 和 Fault Adapter。
