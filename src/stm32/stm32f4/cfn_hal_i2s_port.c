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
 * @file cfn_hal_i2s_port.c
 * @brief STM32F4 I2S HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_i2s_port.h"
#include "cfn_hal_clock.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_i2s.h"
#include "cfn_hal_stm32_error.h"
#include "stm32f4xx_hal.h"

#ifdef HAL_I2S_MODULE_ENABLED

/* Private Data -----------------------------------------------------*/

static SPI_TypeDef *const PORT_INSTANCES[CFN_HAL_I2S_PORT_MAX] = {
#if defined(SPI2)
    [CFN_HAL_I2S_PORT_2] = SPI2,
#endif
#if defined(SPI3)
    [CFN_HAL_I2S_PORT_3] = SPI3,
#endif
};

static I2S_HandleTypeDef port_hi2ss[CFN_HAL_I2S_PORT_MAX];
static cfn_hal_i2s_t    *port_drivers[CFN_HAL_I2S_PORT_MAX];

/* Internal Helpers -------------------------------------------------*/

static uint32_t get_port_id_from_handle(I2S_HandleTypeDef *hi2s)
{
    if ((hi2s < &port_hi2ss[0]) || (hi2s >= &port_hi2ss[CFN_HAL_I2S_PORT_MAX]))
    {
        return UINT32_MAX;
    }
    return (uint32_t) (hi2s - port_hi2ss);
}

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t low_level_init(cfn_hal_i2s_t *driver)
{
    if (driver == NULL || driver->phy == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id >= CFN_HAL_I2S_PORT_MAX)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (PORT_INSTANCES[port_id] == SPI2)
    {
        __HAL_RCC_SPI2_CLK_ENABLE();
    }
    else if (PORT_INSTANCES[port_id] == SPI3)
    {
        __HAL_RCC_SPI3_CLK_ENABLE();
    }

    /* 2. Initialize Pins */
    if (driver->phy->ck)
    {
        (void) cfn_hal_gpio_init(driver->phy->ck->port);
    }
    if (driver->phy->ws)
    {
        (void) cfn_hal_gpio_init(driver->phy->ws->port);
    }
    if (driver->phy->sd)
    {
        (void) cfn_hal_gpio_init(driver->phy->sd->port);
    }
    if (driver->phy->ext_sd)
    {
        (void) cfn_hal_gpio_init(driver->phy->ext_sd->port);
    }
    if (driver->phy->mck)
    {
        (void) cfn_hal_gpio_init(driver->phy->mck->port);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_i2s_t     *driver  = (cfn_hal_i2s_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    I2S_HandleTypeDef *hi2s    = &port_hi2ss[port_id];

    cfn_hal_error_code_t error = low_level_init(driver);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }

    hi2s->Instance         = PORT_INSTANCES[port_id];
    hi2s->Init.Mode        = I2S_MODE_MASTER_TX; /* Default, can be changed via config if needed */
    hi2s->Init.Standard    = I2S_STANDARD_PHILIPS;
    hi2s->Init.DataFormat  = I2S_DATAFORMAT_16B;
    hi2s->Init.MCLKOutput  = I2S_MCLKOUTPUT_ENABLE;
    hi2s->Init.AudioFreq   = driver->config->sample_rate;
    hi2s->Init.CPOL        = I2S_CPOL_LOW;
    hi2s->Init.ClockSource = I2S_CLOCK_PLL;

    return cfn_hal_stm32_map_error(HAL_I2S_Init(hi2s));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_i2s_t *driver  = (cfn_hal_i2s_t *) base;
    uint32_t       port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_I2S_DeInit(&port_hi2ss[port_id]));
}

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(config);
    return port_base_init(base);
}

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask)
{
    CFN_HAL_UNUSED(base);
    if (event_mask != NULL)
    {
        *event_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    CFN_HAL_UNUSED(base);
    if (error_mask != NULL)
    {
        *error_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

/* ST HAL Callback Overrides ----------------------------------------*/

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    uint32_t port_id = get_port_id_from_handle(hi2s);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_i2s_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_I2S_EVENT_TX_COMPLETE, 0, driver->cb_user_arg);
        }
    }
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    uint32_t port_id = get_port_id_from_handle(hi2s);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_i2s_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_I2S_EVENT_RX_READY, 0, driver->cb_user_arg);
        }
    }
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s)
{
    uint32_t port_id = get_port_id_from_handle(hi2s);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_i2s_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_I2S_EVENT_NONE, CFN_HAL_I2S_ERROR_GENERAL, driver->cb_user_arg);
        }
    }
}

/* I2S Specific Functions */

static cfn_hal_error_code_t port_i2s_transmit_dma(cfn_hal_i2s_t *driver, const uint16_t *data, size_t size)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_I2S_Transmit_DMA(&port_hi2ss[port_id], (uint16_t *) data, (uint16_t) size));
}

static cfn_hal_error_code_t port_i2s_receive_dma(cfn_hal_i2s_t *driver, uint16_t *data, size_t size)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_I2S_Receive_DMA(&port_hi2ss[port_id], data, (uint16_t) size));
}

static cfn_hal_error_code_t port_i2s_pause(cfn_hal_i2s_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_I2S_DMAPause(&port_hi2ss[port_id]));
}

static cfn_hal_error_code_t port_i2s_resume(cfn_hal_i2s_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_I2S_DMAResume(&port_hi2ss[port_id]));
}

static cfn_hal_error_code_t port_i2s_stop(cfn_hal_i2s_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_I2S_DMAStop(&port_hi2ss[port_id]));
}

/* API --------------------------------------------------------------*/
static const cfn_hal_i2s_api_t I2S_API = {
    .base =
        {
            .init = port_base_init,
            .deinit = port_base_deinit,
            .power_state_set = NULL,
            .config_set = port_base_config_set,
            .config_validate = NULL,
            .callback_register = NULL,
            .event_enable = NULL,
            .event_disable = NULL,
            .event_get = port_base_event_get,
            .error_enable = NULL,
            .error_disable = NULL,
            .error_get = port_base_error_get,
        },
    .transmit_dma = port_i2s_transmit_dma,
    .receive_dma = port_i2s_receive_dma,
    .pause = port_i2s_pause,
    .resume = port_i2s_resume,
    .stop = port_i2s_stop};

#endif /* HAL_I2S_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/
cfn_hal_error_code_t cfn_hal_i2s_construct(cfn_hal_i2s_t *driver, const cfn_hal_i2s_config_t *config, const cfn_hal_i2s_phy_t *phy, struct cfn_hal_clock_s *clock, cfn_hal_i2s_callback_t callback, void *user_arg)
{
#ifdef HAL_I2S_MODULE_ENABLED
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_I2S_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    cfn_hal_i2s_populate(driver, clock, &I2S_API, phy, config, callback, user_arg);

    port_hi2ss[port_id].Instance = PORT_INSTANCES[port_id];
    port_drivers[port_id]        = driver;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    CFN_HAL_UNUSED(phy);
    CFN_HAL_UNUSED(clock);
    CFN_HAL_UNUSED(callback);
    CFN_HAL_UNUSED(user_arg);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

cfn_hal_error_code_t cfn_hal_i2s_destruct(cfn_hal_i2s_t *driver)
{
#ifdef HAL_I2S_MODULE_ENABLED
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (driver->phy != NULL)
    {
        uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
        if (port_id < CFN_HAL_I2S_PORT_MAX)
        {
            port_drivers[port_id] = NULL;
        }
    }

    driver->config = NULL;
    driver->phy    = NULL;
    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}
