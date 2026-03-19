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
 * @file cfn_hal_uart.c
 * @brief POSIX (Linux) UART HAL Implementation with Dummy RTOS Locking.
 */

/* Includes ---------------------------------------------------------*/
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

#include "cfn_hal_uart.h"
#include "cfn_hal_uart_port.h"

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    CFN_HAL_UNUSED(base);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    CFN_HAL_UNUSED(base);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_power_state_set(cfn_hal_driver_t *base, cfn_hal_power_state_t state)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(state);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(config);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_base_callback_register(cfn_hal_driver_t *base, cfn_hal_callback_t callback, void *user_arg)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(callback);
    CFN_HAL_UNUSED(user_arg);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_enable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(event_mask);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(event_mask);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask)
{
    CFN_HAL_UNUSED(base);
    if (event_mask)
    {
        *event_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(error_mask);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(error_mask);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    CFN_HAL_UNUSED(base);
    if (error_mask)
    {
        *error_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

#if (CFN_HAL_USE_LOCK == 1)
static cfn_hal_error_code_t port_base_lock(cfn_hal_driver_t *base, uint32_t timeout)
{
    if (!base || !base->lock_obj)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    pthread_mutex_t *mutex = (pthread_mutex_t *) base->lock_obj;

    if (timeout == CFN_HAL_MAX_DELAY)
    {
        if (pthread_mutex_lock(mutex) == 0)
        {
            return CFN_HAL_ERROR_OK;
        }
    }
    else
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout / 1000;
        ts.tv_nsec += (timeout % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000)
        {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }

        int res = pthread_mutex_timedlock(mutex, &ts);
        if (res == 0)
        {
            return CFN_HAL_ERROR_OK;
        }
        else if (res == ETIMEDOUT)
        {
            return CFN_HAL_ERROR_BUSY;
        }
    }

    return CFN_HAL_ERROR_EXTERNAL;
}

static cfn_hal_error_code_t port_base_unlock(cfn_hal_driver_t *base)
{
    if (!base || !base->lock_obj)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    pthread_mutex_unlock((pthread_mutex_t *) base->lock_obj);
    return CFN_HAL_ERROR_OK;
}
#endif

/* UART Specific Extensions */

static cfn_hal_error_code_t port_uart_tx_irq(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(nbr_of_bytes);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_uart_tx_irq_abort(cfn_hal_uart_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_uart_rx_n_irq(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes)
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

static cfn_hal_error_code_t port_uart_rx_irq(cfn_hal_uart_t *driver)
{
    if (driver)
    {
        driver->base.flags |= CFN_HAL_UART_FLAG_CONTINUOUS_RX;
    }
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

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

static cfn_hal_error_code_t port_uart_tx_dma(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(nbr_of_bytes);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_uart_rx_dma(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(nbr_of_bytes);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_uart_api_t UART_API = {
    .base = {
        .init = port_base_init,
        .deinit = port_base_deinit,
        .power_state_set = port_base_power_state_set,
        .config_set = port_base_config_set,
        .callback_register = port_base_callback_register,
        .event_enable = port_base_event_enable,
        .event_disable = port_base_event_disable,
        .event_get = port_base_event_get,
        .error_enable = port_base_error_enable,
        .error_disable = port_base_error_disable,
        .error_get = port_base_error_get,
#if (CFN_HAL_USE_LOCK == 1)
        .lock = port_base_lock,
        .unlock = port_base_unlock,
#endif
    },
    .tx_irq = port_uart_tx_irq,
    .tx_irq_abort = port_uart_tx_irq_abort,
    .rx_n_irq = port_uart_rx_n_irq,
    .rx_irq = port_uart_rx_irq,
    .rx_irq_abort = port_uart_rx_irq_abort,
    .tx_polling = port_uart_tx_polling,
    .rx_polling = port_uart_rx_polling,
    .rx_to_idle = port_uart_rx_to_idle,
    .tx_dma = port_uart_tx_dma,
    .rx_dma = port_uart_rx_dma
};

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_uart_construct(cfn_hal_uart_t *driver, const cfn_hal_uart_config_t *config, const cfn_hal_uart_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

#if (CFN_HAL_USE_LOCK == 1)
    pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
    if (mutex == NULL)
    {
        return CFN_HAL_ERROR_MEMORY_ALLOC;
    }
    if (pthread_mutex_init(mutex, NULL) != 0)
    {
        free(mutex);
        return CFN_HAL_ERROR_EXTERNAL;
    }
    driver->base.lock_obj = mutex;
#endif

    driver->api = &UART_API;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_UART;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_uart_destruct(cfn_hal_uart_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

#if (CFN_HAL_USE_LOCK == 1)
    if (driver->base.lock_obj)
    {
        pthread_mutex_destroy((pthread_mutex_t *) driver->base.lock_obj);
        free(driver->base.lock_obj);
        driver->base.lock_obj = NULL;
    }
#endif

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_UART;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
}
