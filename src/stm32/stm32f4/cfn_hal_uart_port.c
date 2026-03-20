/**
 * Copyright (c) 2026 Hisham Moussa Daou <https://www.whileone.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file cfn_hal_uart_port.c
 * @brief STM32F4 UART HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_uart.h"
#include "cfn_hal_uart_port.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_stm32_error.h"

#ifdef HAL_UART_MODULE_ENABLED

/* Private Data -----------------------------------------------------*/

/**
 * @brief Mapping from Caffeine UART data length configuration to STM32 HAL WordLength.
 */
static const uint32_t PORT_MAP_DATA_LEN[CFN_HAL_UART_CONFIG_DATA_LEN_MAX] = {
    [CFN_HAL_UART_CONFIG_DATA_LEN_5] = UINT32_MAX,         // Not supported
    [CFN_HAL_UART_CONFIG_DATA_LEN_6] = UINT32_MAX,         // Not supported
    [CFN_HAL_UART_CONFIG_DATA_LEN_7] = UINT32_MAX,         // Not supported
    [CFN_HAL_UART_CONFIG_DATA_LEN_8] = UART_WORDLENGTH_8B, // 8 bits
    [CFN_HAL_UART_CONFIG_DATA_LEN_9] = UART_WORDLENGTH_9B, // 9 bits
};

/**
 * @brief Mapping from Caffeine UART stop bits configuration to STM32 HAL StopBits.
 */
static const uint32_t PORT_MAP_STOP_BITS[CFN_HAL_UART_CONFIG_STOP_MAX] = {
    [CFN_HAL_UART_CONFIG_STOP_ONE_BIT]  = UART_STOPBITS_1,
    [CFN_HAL_UART_CONFIG_STOP_TWO_BITS] = UART_STOPBITS_2,
};

/**
 * @brief Mapping from Caffeine UART parity configuration to STM32 HAL Parity.
 */
static const uint32_t PORT_MAP_PARITY[CFN_HAL_UART_CONFIG_PARITY_MAX] = {
    [CFN_HAL_UART_CONFIG_PARITY_NONE] = UART_PARITY_NONE,
    [CFN_HAL_UART_CONFIG_PARITY_EVEN] = UART_PARITY_EVEN,
    [CFN_HAL_UART_CONFIG_PARITY_ODD]  = UART_PARITY_ODD,
};

/**
 * @brief Mapping from Caffeine UART direction configuration to STM32 HAL Mode.
 */
static const uint32_t PORT_MAP_DIRECTION[CFN_HAL_UART_CONFIG_DIRECTION_MAX] = {
    [CFN_HAL_UART_CONFIG_DIRECTION_NONE]    = 0,
    [CFN_HAL_UART_CONFIG_DIRECTION_TX_ONLY] = UART_MODE_TX,
    [CFN_HAL_UART_CONFIG_DIRECTION_RX_ONLY] = UART_MODE_RX,
    [CFN_HAL_UART_CONFIG_DIRECTION_TX_RX]   = UART_MODE_TX_RX,
};

/**
 * @brief Mapping from Caffeine UART flow control configuration to STM32 HAL HwFlowCtl.
 */
static const uint32_t PORT_MAP_FLOW_CONTROL[CFN_HAL_UART_CONFIG_FLOW_CTRL_MAX] = {
    [CFN_HAL_UART_CONFIG_FLOW_CTRL_NONE]    = UART_HWCONTROL_NONE,
    [CFN_HAL_UART_CONFIG_FLOW_CTRL_CTS]     = UART_HWCONTROL_CTS,
    [CFN_HAL_UART_CONFIG_FLOW_CTRL_RTS]     = UART_HWCONTROL_RTS,
    [CFN_HAL_UART_CONFIG_FLOW_CTRL_RTS_CTS] = UART_HWCONTROL_RTS_CTS,
};

static const cfn_hal_port_peripheral_id_t PORT_MAP_CLOCK_PERIPHERAL_ID[CFN_HAL_UART_PORT_MAX] = {
    [CFN_HAL_UART_PORT_USART1] = CFN_HAL_PORT_PERIPH_USART1, [CFN_HAL_UART_PORT_USART2] = CFN_HAL_PORT_PERIPH_USART2,
    [CFN_HAL_UART_PORT_USART3] = CFN_HAL_PORT_PERIPH_USART3, [CFN_HAL_UART_PORT_UART4] = CFN_HAL_PORT_PERIPH_UART4,
    [CFN_HAL_UART_PORT_UART5] = CFN_HAL_PORT_PERIPH_UART5,   [CFN_HAL_UART_PORT_USART6] = CFN_HAL_PORT_PERIPH_USART6,
    [CFN_HAL_UART_PORT_UART7] = CFN_HAL_PORT_PERIPH_UART7,   [CFN_HAL_UART_PORT_UART8] = CFN_HAL_PORT_PERIPH_UART8,
    [CFN_HAL_UART_PORT_UART9] = CFN_HAL_PORT_PERIPH_UART9,   [CFN_HAL_UART_PORT_UART10] = CFN_HAL_PORT_PERIPH_UART10,
};
/**
 * @brief Table of physical peripheral register base addresses.
 */
static USART_TypeDef *const PORT_INSTANCES[CFN_HAL_UART_PORT_MAX] = {
#if defined(USART1)
    [CFN_HAL_UART_PORT_USART1] = USART1,
#endif
#if defined(USART2)
    [CFN_HAL_UART_PORT_USART2] = USART2,
#endif
#if defined(USART3)
    [CFN_HAL_UART_PORT_USART3] = USART3,
#endif
#if defined(UART4)
    [CFN_HAL_UART_PORT_UART4] = UART4,
#endif
#if defined(UART5)
    [CFN_HAL_UART_PORT_UART5] = UART5,
#endif
#if defined(USART6)
    [CFN_HAL_UART_PORT_USART6] = USART6,
#endif
};

/**
 * @brief Array of STM32 HAL UART handles, one for each physical port.
 */
static UART_HandleTypeDef port_huarts[CFN_HAL_UART_PORT_MAX];

/**
 * @brief Array of active Caffeine UART driver pointers, used by ISRs and callbacks.
 */
static cfn_hal_uart_t *port_drivers[CFN_HAL_UART_PORT_MAX];

/* Internal Helpers -------------------------------------------------*/

/**
 * @brief Calculates the port ID from an STM32 HAL UART handle pointer using O(1) arithmetic.
 * @param huart Pointer to the STM32 HAL UART handle.
 * @return The 0-based port ID, or -1 if the handle is not part of the port array.
 */
static int32_t get_port_id_from_handle(UART_HandleTypeDef *huart)
{
    if ((huart < &port_huarts[0]) || (huart >= &port_huarts[CFN_HAL_UART_PORT_MAX]))
    {
        return -1;
    }
    return (int32_t) (huart - port_huarts);
}

/* VMT Implementations ----------------------------------------------*/

/**
 * @brief Performs low-level hardware initialization (Clocks and GPIOs).
 * @param driver Pointer to the UART driver instance.
 */
static enum cfn_hal_error_codes low_level_init(cfn_hal_uart_t *driver)
{
    if (driver == NULL || driver->phy == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;

    if (port_id >= CFN_HAL_UART_PORT_MAX)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    cfn_hal_port_clock_enable_gate(PORT_MAP_CLOCK_PERIPHERAL_ID[port_id]);

    if (driver->phy->tx)
    {
        (void) cfn_hal_gpio_init(driver->phy->tx->port);
    }
    if (driver->phy->rx)
    {
        (void) cfn_hal_gpio_init(driver->phy->rx->port);
    }

    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Base initialization for the STM32 UART peripheral.
 * @param base Pointer to the base driver structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    if (base == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_uart_t     *driver  = (cfn_hal_uart_t *) base;
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    UART_HandleTypeDef *huart   = &port_huarts[port_id];

    if (driver->config == NULL ||
        ((driver->config->data_len != CFN_HAL_UART_CONFIG_DATA_LEN_8) &&
         (driver->config->data_len != CFN_HAL_UART_CONFIG_DATA_LEN_9)) ||
        (driver->config->stop_bits >= CFN_HAL_UART_CONFIG_STOP_MAX) ||
        (driver->config->parity >= CFN_HAL_UART_CONFIG_PARITY_MAX) ||
        (driver->config->direction >= CFN_HAL_UART_CONFIG_DIRECTION_MAX) ||
        (driver->config->flow_ctrl >= CFN_HAL_UART_CONFIG_FLOW_CTRL_MAX))
    {
        return CFN_HAL_ERROR_BAD_CONFIG;
    }

    cfn_hal_error_code_t error = low_level_init(driver);

    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }

    huart->Instance          = PORT_INSTANCES[port_id];
    huart->Init.BaudRate     = driver->config->baudrate;
    huart->Init.WordLength   = PORT_MAP_DATA_LEN[driver->config->data_len];
    huart->Init.StopBits     = PORT_MAP_STOP_BITS[driver->config->stop_bits];
    huart->Init.Parity       = PORT_MAP_PARITY[driver->config->parity];
    huart->Init.Mode         = PORT_MAP_DIRECTION[driver->config->direction];
    huart->Init.HwFlowCtl    = PORT_MAP_FLOW_CONTROL[driver->config->flow_ctrl];
    huart->Init.OverSampling = UART_OVERSAMPLING_16;

    return cfn_hal_stm32_map_error(HAL_UART_Init(huart));
}

/**
 * @brief Base deinitialization for the STM32 UART peripheral.
 * @param base Pointer to the base driver structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_uart_t *driver  = (cfn_hal_uart_t *) base;
    uint32_t        port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_UART_DeInit(&port_huarts[port_id]));
}

/**
 * @brief Updates the UART configuration at runtime.
 * @param base Pointer to the base driver structure.
 * @param config Pointer to the new UART configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(config);
    return port_base_init(base);
}

/**
 * @brief Enables specific UART interrupt events.
 * @param base Pointer to the base driver structure.
 * @param event_mask Bitmask of nominal events to enable.
 * @return CFN_HAL_ERROR_OK on success.
 */
static cfn_hal_error_code_t port_base_event_enable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    cfn_hal_uart_t     *driver  = (cfn_hal_uart_t *) base;
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    UART_HandleTypeDef *huart   = &port_huarts[port_id];

    if (event_mask & CFN_HAL_UART_EVENT_RX_READY)
    {
        __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
    }
    if (event_mask & CFN_HAL_UART_EVENT_TX_COMPLETE)
    {
        __HAL_UART_ENABLE_IT(huart, UART_IT_TC);
    }
    if (event_mask & CFN_HAL_UART_EVENT_BUS_IDLE)
    {
        __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
    }

    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Disables specific UART interrupt events.
 * @param base Pointer to the base driver structure.
 * @param event_mask Bitmask of nominal events to disable.
 * @return CFN_HAL_ERROR_OK on success.
 */
static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    cfn_hal_uart_t     *driver  = (cfn_hal_uart_t *) base;
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    UART_HandleTypeDef *huart   = &port_huarts[port_id];

    if (event_mask & CFN_HAL_UART_EVENT_RX_READY)
    {
        __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
    }
    if (event_mask & CFN_HAL_UART_EVENT_TX_COMPLETE)
    {
        __HAL_UART_DISABLE_IT(huart, UART_IT_TC);
    }
    if (event_mask & CFN_HAL_UART_EVENT_BUS_IDLE)
    {
        __HAL_UART_DISABLE_IT(huart, UART_IT_IDLE);
    }

    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Gets the current status of UART interrupt flags.
 * @param base Pointer to the base driver structure.
 * @param event_mask Pointer to store the bitmask of active events.
 * @return CFN_HAL_ERROR_OK on success.
 */
static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask)
{
    cfn_hal_uart_t     *driver  = (cfn_hal_uart_t *) base;
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    UART_HandleTypeDef *huart   = &port_huarts[port_id];
    uint32_t            mask    = 0;

    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE))
    {
        mask |= CFN_HAL_UART_EVENT_RX_READY;
    }
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_TC))
    {
        mask |= CFN_HAL_UART_EVENT_TX_COMPLETE;
    }
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))
    {
        mask |= CFN_HAL_UART_EVENT_BUS_IDLE;
    }

    if (event_mask != NULL)
    {
        *event_mask = mask;
    }
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Enables specific UART exception error interrupts.
 * @param base Pointer to the base driver structure.
 * @param error_mask Bitmask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success.
 */
static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_uart_t     *driver  = (cfn_hal_uart_t *) base;
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    UART_HandleTypeDef *huart   = &port_huarts[port_id];

    if (error_mask & CFN_HAL_UART_ERROR_PARITY)
    {
        __HAL_UART_ENABLE_IT(huart, UART_IT_PE);
    }
    if (error_mask & (CFN_HAL_UART_ERROR_FRAMING | CFN_HAL_UART_ERROR_OVERRUN))
    {
        __HAL_UART_ENABLE_IT(huart, UART_IT_ERR);
    }

    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Disables specific UART exception error interrupts.
 * @param base Pointer to the base driver structure.
 * @param error_mask Bitmask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success.
 */
static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_uart_t     *driver  = (cfn_hal_uart_t *) base;
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    UART_HandleTypeDef *huart   = &port_huarts[port_id];

    if (error_mask & CFN_HAL_UART_ERROR_PARITY)
    {
        __HAL_UART_DISABLE_IT(huart, UART_IT_PE);
    }
    if (error_mask & (CFN_HAL_UART_ERROR_FRAMING | CFN_HAL_UART_ERROR_OVERRUN))
    {
        __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);
    }

    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Gets the current status of UART error flags.
 * @param base Pointer to the base driver structure.
 * @param error_mask Pointer to store the bitmask of active errors.
 * @return CFN_HAL_ERROR_OK on success.
 */
static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    cfn_hal_uart_t     *driver  = (cfn_hal_uart_t *) base;
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    UART_HandleTypeDef *huart   = &port_huarts[port_id];
    uint32_t            mask    = 0;

    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_PE))
    {
        mask |= CFN_HAL_UART_ERROR_PARITY;
    }
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_FE))
    {
        mask |= CFN_HAL_UART_ERROR_FRAMING;
    }
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE))
    {
        mask |= CFN_HAL_UART_ERROR_OVERRUN;
    }

    if (error_mask != NULL)
    {
        *error_mask = mask;
    }
    return CFN_HAL_ERROR_OK;
}

/* ST HAL Callback Overrides ----------------------------------------*/

/**
 * @brief Override of the ST HAL Tx Complete callback to fire Caffeine events.
 * @param huart Pointer to the ST HAL UART handle.
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    int32_t port_id = get_port_id_from_handle(huart);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_uart_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_UART_EVENT_TX_COMPLETE, CFN_HAL_UART_ERROR_NONE, NULL, 0, driver->cb_user_arg);
        }
    }
}

/**
 * @brief Override of the ST HAL Rx Complete callback to fire Caffeine events.
 * @param huart Pointer to the ST HAL UART handle.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    int32_t port_id = get_port_id_from_handle(huart);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_uart_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_UART_EVENT_RX_READY, CFN_HAL_UART_ERROR_NONE, NULL, 0, driver->cb_user_arg);
        }
    }
}

/**
 * @brief Override of the ST HAL Error callback to fire Caffeine events.
 * @param huart Pointer to the ST HAL UART handle.
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    int32_t port_id = get_port_id_from_handle(huart);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_uart_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            uint32_t error_mask = 0;
            (void) port_base_error_get(&driver->base, &error_mask);
            driver->cb(driver, CFN_HAL_UART_EVENT_NONE, error_mask, NULL, 0, driver->cb_user_arg);
        }
    }
}

/**
 * @brief Override of the ST HAL Rx Event callback (Idle/Half-Full) to fire Caffeine events.
 * @param huart Pointer to the ST HAL UART handle.
 * @param size Number of bytes received so far.
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    int32_t port_id = get_port_id_from_handle(huart);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_uart_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            /* Notify RX READY with the actual size received */
            driver->cb(
                driver, CFN_HAL_UART_EVENT_RX_READY, CFN_HAL_UART_ERROR_NONE, NULL, (size_t) size, driver->cb_user_arg);
        }
    }
}

/* Raw ISR Handlers -------------------------------------------------*/

#ifndef CFN_HAL_PORT_DISABLE_IRQ_UART

/**
 * @brief Unified internal ISR handler for all UART instances.
 * @param port_id The physical UART port ID.
 */
static void port_uart_handle_isr(cfn_hal_uart_port_t port_id)
{
    UART_HandleTypeDef *huart  = &port_huarts[port_id];
    cfn_hal_uart_t     *driver = port_drivers[port_id];

    if (driver != NULL && huart != NULL)
    {
        uint32_t isrflags = READ_REG(huart->Instance->SR);
        uint32_t cr1its   = READ_REG(huart->Instance->CR1);

        if ((driver->base.flags & CFN_HAL_UART_FLAG_CONTINUOUS_RX) && ((isrflags & USART_SR_RXNE) != RESET) &&
            ((cr1its & USART_CR1_RXNEIE) != RESET))
        {
            uint32_t error_mask = CFN_HAL_UART_ERROR_NONE;
            if (isrflags & USART_SR_ORE)
            {
                error_mask |= CFN_HAL_UART_ERROR_OVERRUN;
            }
            if (isrflags & USART_SR_FE)
            {
                error_mask |= CFN_HAL_UART_ERROR_FRAMING;
            }
            if (isrflags & USART_SR_NE)
            {
                error_mask |= CFN_HAL_UART_ERROR_GENERAL;
            }
            if (isrflags & USART_SR_PE)
            {
                error_mask |= CFN_HAL_UART_ERROR_PARITY;
            }

            /* Read DR to get byte and clear RXNE flag. */
#if defined(USART_SR_RXNE)
            uint8_t byte = (uint8_t) (huart->Instance->DR & (uint8_t) 0x00FF);
#else
            uint8_t byte = (uint8_t) (huart->Instance->RDR & (uint8_t) 0x00FF);
#endif
            if (driver->cb)
            {
                driver->cb(driver, CFN_HAL_UART_EVENT_RX_BYTE, error_mask, &byte, 1, driver->cb_user_arg);
            }
            /* Return early to avoid HAL overhead. Any concurrent interrupt sources
               (TXE, TC, etc.) will cause the NVIC to immediately re-trigger the ISR. */
            return;
        }
    }
    HAL_UART_IRQHandler(huart);
}

#if defined(USART1)
/**
 * @brief USART1 Interrupt Handler.
 */
void USART1_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    port_uart_handle_isr(CFN_HAL_UART_PORT_USART1);
}
#endif

#if defined(USART2)
/**
 * @brief USART2 Interrupt Handler.
 */
void USART2_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    port_uart_handle_isr(CFN_HAL_UART_PORT_USART2);
}
#endif

#if defined(USART3)
/**
 * @brief USART3 Interrupt Handler.
 */
void USART3_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    port_uart_handle_isr(CFN_HAL_UART_PORT_USART3);
}
#endif

#if defined(UART4)
/**
 * @brief UART4 Interrupt Handler.
 */
void UART4_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    port_uart_handle_isr(CFN_HAL_UART_PORT_UART4);
}
#endif

#if defined(UART5)
/**
 * @brief UART5 Interrupt Handler.
 */
void UART5_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    port_uart_handle_isr(CFN_HAL_UART_PORT_UART5);
}
#endif

#if defined(USART6)
/**
 * @brief USART6 Interrupt Handler.
 */
void USART6_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    port_uart_handle_isr(CFN_HAL_UART_PORT_USART6);
}
#endif

#endif /* CFN_HAL_PORT_DISABLE_IRQ_UART */

/* UART Specific Functions */

/**
 * @brief Transmits data in blocking polling mode.
 * @param driver Pointer to the UART driver instance.
 * @param data Pointer to the buffer containing data to transmit.
 * @param length Number of bytes to transmit.
 * @param timeout Timeout in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t
port_uart_tx_polling(cfn_hal_uart_t *driver, const uint8_t *data, size_t length, uint32_t timeout)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(
        HAL_UART_Transmit(&port_huarts[port_id], (uint8_t *) data, (uint16_t) length, timeout));
}

/**
 * @brief Receives data in blocking polling mode.
 * @param driver Pointer to the UART driver instance.
 * @param buffer Pointer to the buffer to store received data.
 * @param length Number of bytes to receive.
 * @param timeout Timeout in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t
port_uart_rx_polling(cfn_hal_uart_t *driver, uint8_t *buffer, size_t length, uint32_t timeout)
{
    if (driver)
    {
        driver->base.flags &= ~CFN_HAL_UART_FLAG_CONTINUOUS_RX;
    }
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_UART_Receive(&port_huarts[port_id], buffer, (uint16_t) length, timeout));
}

/**
 * @brief Receives data until an IDLE line is detected or the buffer is full.
 * @param driver Pointer to the UART driver instance.
 * @param data Pointer to the buffer to store received data.
 * @param max_bytes Maximum number of bytes to receive.
 * @param received_bytes Pointer to store the actual number of bytes received (optional).
 * @param timeout Timeout in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t
port_uart_rx_to_idle(cfn_hal_uart_t *driver, uint8_t *data, size_t max_bytes, size_t *received_bytes, uint32_t timeout)
{
    CFN_HAL_UNUSED(timeout);
    CFN_HAL_UNUSED(received_bytes);

    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->base.flags &= ~CFN_HAL_UART_FLAG_CONTINUOUS_RX;
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    UART_HandleTypeDef *huart   = &port_huarts[port_id];

    return cfn_hal_stm32_map_error(HAL_UARTEx_ReceiveToIdle_IT(huart, data, (uint16_t) max_bytes));
}

/**
 * @brief Transmits data using interrupts.
 * @param driver Pointer to the UART driver instance.
 * @param data Pointer to the buffer containing data to transmit.
 * @param nbr_of_bytes Number of bytes to transmit.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_tx_irq(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(
        HAL_UART_Transmit_IT(&port_huarts[port_id], (uint8_t *) data, (uint16_t) nbr_of_bytes));
}

/**
 * @brief Aborts an active interrupt-based transmission.
 * @param driver Pointer to the UART driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_tx_irq_abort(cfn_hal_uart_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_UART_AbortTransmit_IT(&port_huarts[port_id]));
}

/**
 * @brief Receives a fixed number of bytes using interrupts.
 * @param driver Pointer to the UART driver instance.
 * @param data Pointer to the buffer to store received data.
 * @param nbr_of_bytes Number of bytes to receive.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_rx_n_irq(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes)
{
    if (driver)
    {
        driver->base.flags &= ~CFN_HAL_UART_FLAG_CONTINUOUS_RX;
    }
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_UART_Receive_IT(&port_huarts[port_id], data, (uint16_t) nbr_of_bytes));
}

/**
 * @brief Enables continuous RX mode using interrupts.
 * @param driver Pointer to the UART driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_rx_irq(cfn_hal_uart_t *driver)
{
    if (!driver || !driver->phy || !driver->phy->instance)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.flags |= CFN_HAL_UART_FLAG_CONTINUOUS_RX;
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    __HAL_UART_ENABLE_IT(&port_huarts[port_id], UART_IT_RXNE);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Aborts continuous RX mode or an active interrupt-based reception.
 * @param driver Pointer to the UART driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_rx_irq_abort(cfn_hal_uart_t *driver)
{
    if (driver)
    {
        driver->base.flags &= ~CFN_HAL_UART_FLAG_CONTINUOUS_RX;
    }
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    __HAL_UART_DISABLE_IT(&port_huarts[port_id], UART_IT_RXNE);
    return cfn_hal_stm32_map_error(HAL_UART_AbortReceive_IT(&port_huarts[port_id]));
}

/**
 * @brief Starts UART data reception using DMA.
 * @param driver Pointer to the UART driver instance.
 * @param data Pointer to the buffer to store received data.
 * @param nbr_of_bytes Number of bytes to receive.
 * @return CFN_HAL_ERROR_NOT_SUPPORTED (Dummy implementation).
 */
static cfn_hal_error_code_t port_uart_rx_dma(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes)
{
    if (driver)
    {
        driver->base.flags &= ~CFN_HAL_UART_FLAG_CONTINUOUS_RX;
    }
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(nbr_of_bytes);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief UART API VMT instance for the STM32F4 port.
 */
static const cfn_hal_uart_api_t UART_API = {
    .base = {
        .init = port_base_init,
        .deinit = port_base_deinit,
        .power_state_set = NULL,
        .config_set = port_base_config_set,
        .callback_register = NULL,
        .event_enable = port_base_event_enable,
        .event_disable = port_base_event_disable,
        .event_get = port_base_event_get,
        .error_enable = port_base_error_enable,
        .error_disable = port_base_error_disable,
        .error_get = port_base_error_get,
    },
    .tx_irq = port_uart_tx_irq,
    .tx_irq_abort = port_uart_tx_irq_abort,
    .rx_n_irq = port_uart_rx_n_irq,
    .rx_irq = port_uart_rx_irq,
    .rx_irq_abort = port_uart_rx_irq_abort,
    .tx_polling = port_uart_tx_polling,
    .rx_polling = port_uart_rx_polling,
    .rx_to_idle = port_uart_rx_to_idle,
    .tx_dma = NULL,
    .rx_dma = port_uart_rx_dma
};

#endif /* HAL_UART_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/

/**
 * @brief Constructs a new UART driver instance.
 * @param driver Pointer to the driver structure to construct.
 * @param config Pointer to the UART configuration.
 * @param phy Pointer to the physical UART mapping.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t
cfn_hal_uart_construct(cfn_hal_uart_t *driver, const cfn_hal_uart_config_t *config, const cfn_hal_uart_phy_t *phy)
{
#ifdef HAL_UART_MODULE_ENABLED
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_UART_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api                   = &UART_API;
    driver->base.type             = CFN_HAL_PERIPHERAL_TYPE_UART;
    driver->base.status           = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config                = config;
    driver->phy                   = phy;

    port_huarts[port_id].Instance = PORT_INSTANCES[port_id];
    port_drivers[port_id]         = driver;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    CFN_HAL_UNUSED(phy);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

/**
 * @brief Destructs an existing UART driver instance.
 * @param driver Pointer to the driver instance to destruct.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_uart_destruct(cfn_hal_uart_t *driver)
{
#ifdef HAL_UART_MODULE_ENABLED
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id < CFN_HAL_UART_PORT_MAX)
    {
        port_drivers[port_id] = NULL;
    }

    driver->api         = NULL;
    driver->base.type   = CFN_HAL_PERIPHERAL_TYPE_UART;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config      = NULL;
    driver->phy         = NULL;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}
