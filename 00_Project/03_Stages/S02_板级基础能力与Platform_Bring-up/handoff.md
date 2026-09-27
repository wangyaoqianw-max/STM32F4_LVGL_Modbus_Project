# S02 Handoff

## Handoff metadata

- Stage: `S02 板级基础能力 / Platform Bring-up`
- Status: `READY_FOR_REVIEW`
- Branch: `main`
- Design Baseline: `0ffe747eeb0a1e7e2c6a088e2b402bbc57965f44`
- Reuse Baseline: `wangyaoqianw-max/Embedded_Engineering_Library@8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0`
- Implementation Commit: Review 前提交；Review 通过后回填
- Verification: `04_Test/Reports/Stages/S02/verification.md`
- Date: `2026-09-27`

## Completed scope

- 复用并接入 Library 的 Platform MCU GPIO、Software I2C、SPI、UART、IRQ、Reset API，以及 STM32F4 Delay/GPIO/SPI/UART/IRQ/Reset Impl。
- 新增薄型 Board Binding：PB6/PB7、PC5、`hspi1`、`huart1/2/3` 映射为 Platform 对象。
- 将所选源文件和 Include Path 接入 Keil；正式目标 `STM32F407_APP` Clean Rebuild 为 `0 Error / 0 Warning`。
- GPIO、Software I2C 地址 ACK、USART1 Blocking TX/RX 均完成真实板级 Smoke；SPI1 Binding 和 Platform 生命周期通过。
- 临时 Smoke 代码和启动入口已清理；正式固件已重新构建并烧录校验。

## Frozen boundaries

- 依赖方向保持 `APP → Service → Platform → Impl → HAL / Vendor`。
- 本阶段只保证 `CubeMX/HAL Resource → STM32F4 Impl → Board/HAL Binding → Platform API` 可用；没有建立完整设备或 Service 初始化链。
- `platform_types.h` 保持冻结 Blob `a2d23e8575f31494e55548bde62c15e7407055b9`。
- 未迁入 Watchdog；未新增 `platform_dma` / `platform_fmc`；未引入设备 Driver、UART Service、RingBuffer 或业务代码。
- Board Binding 只映射资源和当前 HAL 配置，不包含 W25Q128、AT24C02、DHT20、Bluetooth、Modbus、OTA 等业务语义。

## Reuse and F407 adaptation

- 来源资产：Embedded Engineering Library 冻结 Commit `8ae16732fb3be01e4fed0c5d8cdae78c1ad46cd0`；迁入源码与 LICENSE 逐文件一致。
- F407 适配集中在 `04_Impl/impl_board/board_platform_bindings.*`：PB6/PB7 来自当前 CubeMX 宏；PC5 按当前板级 Pinout 状态 LED 资源映射；SPI 与 UART 注入当前 `hspi1`、`huart1/2/3`。
- UART 参数从当前 CubeMX HAL Handle 读取，避免把来源 STM32F411 工程参数当作本项目硬件事实。
- Board 绑定要求在对应 CubeMX GPIO/UART 初始化之后调用；绑定 API 自身不调用 HAL init。
- 冻结 IRQ API 中的 `KEY_EXTI0` 只映射 MCU `EXTI0_IRQn`；当前没有把它绑定到具体按键或调用它，不能据此推断本板按键接线。
- Library `original/LICENSE` 声明 MIT，而迁入源头保留的文件头含 `All Rights Reserved`；源文件按冻结版本保留，授权表述差异已记录，未自行改写。

## Carry-forward

- `DEFERRED`：W25Q128 JEDEC/Read 与 SPI 设备事务进入 S03；UART DMA + IDLE + RingBuffer Runtime 进入 S04；FMC/LCD 实机验证进入 S06。
- `PENDING / TO_VERIFY`：PB6/PB7 外部上拉阻值、实物板卡版本、HSE 实际频率、Pinout `PLL48CLK` 描述与当前 `PLLQCLK=84 MHz` 配置关系。
- `PENDING / TO_VERIFY`：若后续使用冻结 IRQ API 的 `KEY_EXTI0`，先核对本板具体按键及引脚复用；该 API 当前没有消费者。
- USART2/3 已有 CubeMX Handle 和 Board Binding，但没有本阶段阻塞收发 Runtime 结果；若后续被选作 DMA 消费者，在对应阶段验证其 DMA/IRQ 路由。
- 详见 `04_Test/Reports/Stages/S02/verification.md` 的逐项验证状态。

## Next work item

Review PASS 后关闭 S02，切换至路线图中的 `S03 W25Q128 与中文字库基础`。S03 再通过真实 W25Q128 JEDEC / Read 消费者验证 SPI，不在 S02 重复测试。
