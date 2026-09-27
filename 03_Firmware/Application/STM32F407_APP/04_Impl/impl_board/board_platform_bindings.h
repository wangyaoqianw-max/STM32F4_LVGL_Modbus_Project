/******************************************************************************
 * Copyright (C) 2026 YaoQian Wang
 *
 * All Rights Reserved.
 *
 * @file board_platform_bindings.h
 * @brief STM32F407 当前工程的 Platform 资源绑定入口
 * @author YaoQian Wang
 * @date 2026-09-27
 * @version V1.0
 *
 *****************************************************************************/

#ifndef BOARD_PLATFORM_BINDINGS_H
#define BOARD_PLATFORM_BINDINGS_H

#include "platform_error.h"
#include "platform_gpio.h"
#include "platform_spi.h"
#include "platform_uart.h"

/** 当前工程由 Platform GPIO 使用的板级资源。 */
typedef enum
{
    /** 软件 I2C SCL，映射至 PB6。 */
    BOARD_PLATFORM_GPIO_SOFTWARE_I2C_SCL = 0,
    /** 软件 I2C SDA，映射至 PB7。 */
    BOARD_PLATFORM_GPIO_SOFTWARE_I2C_SDA,
    /** 板载状态 LED，映射至 PC5。 */
    BOARD_PLATFORM_GPIO_STATUS_LED,
    BOARD_PLATFORM_GPIO_MAX
} board_platform_gpio_id_t;

/** 当前工程由 Platform UART 使用的 CubeMX UART 资源。 */
typedef enum
{
    /** USART1，对应 CubeMX 生成的 huart1。 */
    BOARD_PLATFORM_UART_USART1 = 0,
    /** USART2，对应 CubeMX 生成的 huart2。 */
    BOARD_PLATFORM_UART_USART2,
    /** USART3，对应 CubeMX 生成的 huart3。 */
    BOARD_PLATFORM_UART_USART3,
    BOARD_PLATFORM_UART_MAX
} board_platform_uart_id_t;

/**
 * @brief 将板级 GPIO 资源构造成 Platform GPIO 对象。
 * @param[in,out] gpio   : 使用 PLATFORM_GPIO_INITIALIZER 初始化的对象
 * @param[in] resource   : 本工程支持的 GPIO 资源 ID
 * @return platform_error_t : 绑定结果
 * @note 每个资源仅绑定到一个对象；对象和内部静态上下文在运行期间保持有效。
 * @note 调用前确保已执行 MX_GPIO_Init()，由 CubeMX 初始化路径开启 GPIO 时钟。
 */
platform_error_t board_platform_gpio_bind(
    platform_gpio_t *gpio,
    board_platform_gpio_id_t resource);

/**
 * @brief 将 CubeMX 的 SPI1 HAL Handle 构造成 Platform SPI Bus 对象。
 * @param[in,out] bus : 使用 PLATFORM_SPI_BUS_INITIALIZER 初始化的对象
 * @return platform_error_t : 绑定结果
 * @note 本函数只绑定 hspi1，不执行 HAL 初始化或 SPI 传输。
 * @note 对象和内部静态上下文在运行期间保持有效。
 */
platform_error_t board_platform_spi1_bind(platform_spi_bus_t *bus);

/**
 * @brief 将 CubeMX UART Handle 和当前 HAL 配置构造成 Platform UART 对象。
 * @param[in,out] uart   : 使用 PLATFORM_UART_INITIALIZER 初始化的对象
 * @param[in] resource   : 本工程支持的 USART 资源 ID
 * @return platform_error_t : 绑定结果
 * @note 调用前需执行对应的 MX_USARTx_Init()；本函数只读取配置并绑定 Handle。
 * @note HAL 初始化由 Platform 生命周期调用。
 * @note 每路 USART 仅绑定到一个对象；对象和内部静态上下文在运行期间保持有效。
 * @note 默认阻塞超时使用 PLATFORM_UART_WAIT_FOREVER。
 */
platform_error_t board_platform_uart_bind(
    platform_uart_t *uart,
    board_platform_uart_id_t resource);

#endif
