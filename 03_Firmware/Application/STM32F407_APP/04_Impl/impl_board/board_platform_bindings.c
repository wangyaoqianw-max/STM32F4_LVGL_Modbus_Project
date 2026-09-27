/******************************************************************************
 * Copyright (C) 2026 YaoQian Wang
 *
 * All Rights Reserved.
 *
 * @file board_platform_bindings.c
 * @brief STM32F407 当前工程的 Platform 资源绑定
 * @author YaoQian Wang
 * @date 2026-09-27
 * @version V1.0
 *
 *****************************************************************************/

#include "board_platform_bindings.h"

#include "impl_platform_gpio.h"
#include "impl_platform_spi.h"
#include "impl_platform_uart.h"

#include "platform_device.h"
#include "main.h"
#include "spi.h"
#include "usart.h"

#include "stm32f4xx_hal.h"

static impl_platform_gpio_context_t s_gpioContexts[BOARD_PLATFORM_GPIO_MAX];
static impl_platform_spi_context_t s_spi1Context;
static impl_platform_uart_context_t s_uartContexts[BOARD_PLATFORM_UART_MAX];

static platform_error_t board_platform_uart_configure(
    UART_HandleTypeDef *halUart,
    platform_uart_config_t *config)
{
    if ((halUart == NULL) || (config == NULL)) {
        return PLATFORM_ERR_INVALID_PARAM;
    }

    config->baudRate = halUart->Init.BaudRate;
    switch (halUart->Init.StopBits) {
        case UART_STOPBITS_1:
            config->stopBits = PLATFORM_UART_STOP_BITS_1;
            break;
        case UART_STOPBITS_2:
            config->stopBits = PLATFORM_UART_STOP_BITS_2;
            break;
        default:
            return PLATFORM_ERR_NOT_SUPPORTED;
    }

    switch (halUart->Init.Parity) {
        case UART_PARITY_NONE:
            config->parity = PLATFORM_UART_PARITY_NONE;
            break;
        case UART_PARITY_EVEN:
            config->parity = PLATFORM_UART_PARITY_EVEN;
            break;
        case UART_PARITY_ODD:
            config->parity = PLATFORM_UART_PARITY_ODD;
            break;
        default:
            return PLATFORM_ERR_NOT_SUPPORTED;
    }

    /* HAL 的 WordLength 包含校验位；Platform dataBits 表示有效数据位。 */
    if (halUart->Init.WordLength == UART_WORDLENGTH_8B) {
        config->dataBits = (config->parity == PLATFORM_UART_PARITY_NONE) ?
                           PLATFORM_UART_DATA_BITS_8 :
                           PLATFORM_UART_DATA_BITS_7;
    } else if (halUart->Init.WordLength == UART_WORDLENGTH_9B) {
        config->dataBits = (config->parity == PLATFORM_UART_PARITY_NONE) ?
                           PLATFORM_UART_DATA_BITS_9 :
                           PLATFORM_UART_DATA_BITS_8;
    } else {
        return PLATFORM_ERR_NOT_SUPPORTED;
    }

    switch (halUart->Init.HwFlowCtl) {
        case UART_HWCONTROL_NONE:
            config->flowControl = PLATFORM_UART_FLOW_CONTROL_NONE;
            break;
        case UART_HWCONTROL_RTS:
            config->flowControl = PLATFORM_UART_FLOW_CONTROL_RTS;
            break;
        case UART_HWCONTROL_CTS:
            config->flowControl = PLATFORM_UART_FLOW_CONTROL_CTS;
            break;
        case UART_HWCONTROL_RTS_CTS:
            config->flowControl = PLATFORM_UART_FLOW_CONTROL_RTS_CTS;
            break;
        default:
            return PLATFORM_ERR_NOT_SUPPORTED;
    }

    config->defaultTimeoutMs = PLATFORM_UART_WAIT_FOREVER;
    return PLATFORM_ERR_OK;
}

platform_error_t board_platform_gpio_bind(
    platform_gpio_t *gpio,
    board_platform_gpio_id_t resource)
{
    impl_platform_gpio_context_t *context;
    const char *name;

    if ((gpio == NULL) ||
        ((uint32_t)resource >= (uint32_t)BOARD_PLATFORM_GPIO_MAX)) {
        return PLATFORM_ERR_INVALID_PARAM;
    }

    context = &s_gpioContexts[resource];
    switch (resource) {
        case BOARD_PLATFORM_GPIO_SOFTWARE_I2C_SCL:
            context->port = I2C_SCL_GPIO_Port;
            context->pin = I2C_SCL_Pin;
            name = "PB6";
            break;
        case BOARD_PLATFORM_GPIO_SOFTWARE_I2C_SDA:
            context->port = I2C_SDA_GPIO_Port;
            context->pin = I2C_SDA_Pin;
            name = "PB7";
            break;
        case BOARD_PLATFORM_GPIO_STATUS_LED:
            context->port = GPIOC;
            context->pin = GPIO_PIN_5;
            name = "PC5";
            break;
        default:
            return PLATFORM_ERR_INVALID_PARAM;
    }

    return impl_platform_gpio_construct(gpio, name, context);
}

platform_error_t board_platform_spi1_bind(platform_spi_bus_t *bus)
{
    s_spi1Context.halSpi = &hspi1;
    return impl_platform_spi_construct(bus,
                                       "SPI1",
                                       PLATFORM_DEVICE_CAP_NONE,
                                       &s_spi1Context);
}

platform_error_t board_platform_uart_bind(
    platform_uart_t *uart,
    board_platform_uart_id_t resource)
{
    static const char *const names[BOARD_PLATFORM_UART_MAX] = {
        "USART1",
        "USART2",
        "USART3"
    };
    UART_HandleTypeDef *halUart;
    platform_uart_config_t config;
    platform_error_t result;

    if ((uart == NULL) ||
        ((uint32_t)resource >= (uint32_t)BOARD_PLATFORM_UART_MAX)) {
        return PLATFORM_ERR_INVALID_PARAM;
    }

    switch (resource) {
        case BOARD_PLATFORM_UART_USART1:
            halUart = &huart1;
            break;
        case BOARD_PLATFORM_UART_USART2:
            halUart = &huart2;
            break;
        case BOARD_PLATFORM_UART_USART3:
            halUart = &huart3;
            break;
        default:
            return PLATFORM_ERR_INVALID_PARAM;
    }

    result = board_platform_uart_configure(halUart, &config);
    if (result != PLATFORM_ERR_OK) {
        return result;
    }

    s_uartContexts[resource].halUart = halUart;
    return impl_platform_uart_construct(uart,
                                        names[resource],
                                        PLATFORM_DEVICE_CAP_NONE,
                                        &config,
                                        NULL,
                                        NULL,
                                        &s_uartContexts[resource]);
}
