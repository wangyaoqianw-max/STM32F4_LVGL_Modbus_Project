# 测试与验证

按证据类型组织测试，并将阶段验收条件与实际证据对应起来：

- `Host`：可在 PC 上验证的算法、协议和工具逻辑；
- `Board`：依赖真实 MCU、外设、Probe 或串口的板级验证；
- `Integration`：多个模块协作和端到端流程；
- `Test_Plans`：可重复执行的测试步骤、输入和预期结果；
- `Reports/Stages/<stage>/verification.md`：阶段验证命令、结果、日志位置、代码/硬件结论及未验证项。

编译或工具链动作通过不等于功能验证通过。无硬件证据时明确记录 `PENDING` 或 `NOT_APPLICABLE`，不推断板级功能结果。
