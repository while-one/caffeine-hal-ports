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
 * @brief UART HAL Template Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_uart.h"
#include "cfn_hal_uart_port.h"

/**
 * @brief Port specific implementation for uart_register_cb.
 *
 * @param driver Parameter description.
 * @param cb Parameter description.
 * @param user_arg Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_register_cb(cfn_hal_uart_t *driver, cfn_hal_uart_callback_t cb, void *user_arg)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(cb);
    CFN_HAL_UNUSED(user_arg);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_set_cb_arg.
 *
 * @param driver Parameter description.
 * @param user_arg Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_set_cb_arg(cfn_hal_uart_t *driver, void *user_arg)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(user_arg);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_tx_irq.
 *
 * @param driver Parameter description.
 * @param data Parameter description.
 * @param nbr_of_bytes Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_tx_irq(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(nbr_of_bytes);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_tx_irq_abort.
 *
 * @param driver Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_tx_irq_abort(cfn_hal_uart_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_rx_irq.
 *
 * @param driver Parameter description.
 * @param data Parameter description.
 * @param nbr_of_bytes Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_rx_irq(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(nbr_of_bytes);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_rx_irq_abort.
 *
 * @param driver Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_rx_irq_abort(cfn_hal_uart_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t
port_uart_tx_polling(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(nbr_of_bytes);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_rx_polling.
 *
 * @param driver Parameter description.
 * @param data Parameter description.
 * @param nbr_of_bytes Parameter description.
 * @param timeout Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t
port_uart_rx_polling(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(nbr_of_bytes);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t
port_uart_rx_to_idle(cfn_hal_uart_t *driver, uint8_t *data, size_t max_bytes, size_t *received_bytes, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(max_bytes);
    CFN_HAL_UNUSED(received_bytes);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_check_error.
 *
 * @param driver Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_check_error(cfn_hal_uart_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_init.
 *
 * @param driver Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_init(cfn_hal_uart_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_deinit.
 *
 * @param driver Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_deinit(cfn_hal_uart_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_cfg_get.
 *
 * @param driver Parameter description.
 * @param config Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_cfg_get(cfn_hal_uart_t *driver, cfn_hal_uart_config_t *config)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_cfg_set.
 *
 * @param driver Parameter description.
 * @param config Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_cfg_set(cfn_hal_uart_t *driver, const cfn_hal_uart_config_t *config)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_cfg_tx_enable.
 *
 * @param driver Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_cfg_tx_enable(cfn_hal_uart_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_cfg_rx_enable.
 *
 * @param driver Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_cfg_rx_enable(cfn_hal_uart_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_cfg_irq_enable.
 *
 * @param driver Parameter description.
 * @param irq Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_cfg_irq_enable(cfn_hal_uart_t *driver, cfn_hal_uart_interrupts_t irq)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(irq);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_cfg_tx_disable.
 *
 * @param driver Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_cfg_tx_disable(cfn_hal_uart_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_cfg_rx_disable.
 *
 * @param driver Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_cfg_rx_disable(cfn_hal_uart_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_cfg_irq_disable.
 *
 * @param driver Parameter description.
 * @param irq Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_cfg_irq_disable(cfn_hal_uart_t *driver, cfn_hal_uart_interrupts_t irq)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(irq);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_tx_dma.
 *
 * @param driver Parameter description.
 * @param data Parameter description.
 * @param nbr_of_bytes Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_tx_dma(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(nbr_of_bytes);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/**
 * @brief Port specific implementation for uart_rx_dma.
 *
 * @param driver Parameter description.
 * @param data Parameter description.
 * @param nbr_of_bytes Parameter description.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static cfn_hal_error_code_t port_uart_rx_dma(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(nbr_of_bytes);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_uart_api_t UART_API = { .cfn_hal_uart_register_cb = port_uart_register_cb,
                                             .cfn_hal_uart_set_cb_arg = port_uart_set_cb_arg,
                                             .cfn_hal_uart_tx_irq = port_uart_tx_irq,
                                             .cfn_hal_uart_tx_irq_abort = port_uart_tx_irq_abort,
                                             .cfn_hal_uart_rx_irq = port_uart_rx_irq,
                                             .cfn_hal_uart_rx_irq_abort = port_uart_rx_irq_abort,
                                             .cfn_hal_uart_tx_polling = port_uart_tx_polling,
                                             .cfn_hal_uart_rx_polling = port_uart_rx_polling,
                                             .cfn_hal_uart_rx_to_idle = port_uart_rx_to_idle,
                                             .cfn_hal_uart_check_error = port_uart_check_error,
                                             .cfn_hal_uart_init = port_uart_init,
                                             .cfn_hal_uart_deinit = port_uart_deinit,
                                             .cfn_hal_uart_cfg_get = port_uart_cfg_get,
                                             .cfn_hal_uart_cfg_set = port_uart_cfg_set,
                                             .cfn_hal_uart_cfg_tx_enable = port_uart_cfg_tx_enable,
                                             .cfn_hal_uart_cfg_rx_enable = port_uart_cfg_rx_enable,
                                             .cfn_hal_uart_cfg_irq_enable = port_uart_cfg_irq_enable,
                                             .cfn_hal_uart_cfg_tx_disable = port_uart_cfg_tx_disable,
                                             .cfn_hal_uart_cfg_rx_disable = port_uart_cfg_rx_disable,
                                             .cfn_hal_uart_cfg_irq_disable = port_uart_cfg_irq_disable,
                                             .cfn_hal_uart_tx_dma = port_uart_tx_dma,
                                             .cfn_hal_uart_rx_dma = port_uart_rx_dma };

/* Instantiation ----------------------------------------------------*/
/**
 * @brief Construct the uart driver.
 *
 * @warning The configuration pointer is stored internally by reference, NOT by value.
 * The caller MUST ensure that the memory address of the configuration structure
 * remains valid for the entire lifecycle of the driver. It is highly recommended
 * to declare the configuration structure as `static` or global to prevent
 * dangling pointers.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t hal_uart_construct(cfn_hal_uart_t *driver, const cfn_hal_uart_config_t *config)
{
    if (driver == NULL || config == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &UART_API;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_UART;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;

    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Destruct the uart driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t hal_uart_destruct(cfn_hal_uart_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_UART;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;

    return CFN_HAL_ERROR_OK;
}
