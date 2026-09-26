# 工程准备

工程准备用于正式功能设计前的输入收集、硬件事实核对和开发环境基线建立。

## 状态

- Work Item: `PREPARATION / S00`
- Result: `COMPLETED FOR S01 ENTRY`
- Exit Baseline: `fd197f695f0742627470cb54068aadc72a238477`
- Date: 2026-09-26

当前已达到进入 S01 的条件：

- STM32F407VET6 原理图、Pinout 和主要板载外设连接已经形成可追溯资料；
- LCD/FSMC、XPT2046、W25Q128、软件 I2C、USART1/2/3、DMA/IRQ 和 SWD 已建立 CubeMX 配置基线；
- Application CubeMX/Keil 母工程已经生成；
- Keil Build、C 代码规范和 RTT/CmBacktrace 迁移参考文档已经进入 `03_Firmware/00_Doc/`；
- 未确认信息继续保留为 `Unknown / TO_VERIFY`，没有为了进入施工而强行补全。

## 继续携带的待验证事项

以下内容不阻塞 S01，但在相关功能阶段必须继续关闭：

- HSE 实际晶振频率；
- PB6/PB7 外部上拉和外接 DHT20 后的电气条件；
- HC-05 实际 UART 参数；
- Modbus 正式 UART 参数；
- RS485 收发方向控制的实际板级实现；
- LCD/FSMC 时序的真实板级验证。

原始资料继续分别放入 `01_Reference` 和 `02_Hardware`。结构化事实应只维护一个正式数据源，避免重复表格长期漂移。

当前活动阶段已经切换到：

`00_Project/03_Stages/S01_Application工程初始化与诊断基础/`
