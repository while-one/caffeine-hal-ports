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
 * @file cfn_hal_comp_port.c
 * @brief STM32F4 COMP HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_comp_port.h"
#include "cfn_hal_clock.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_comp.h"
#include "stm32f4xx_hal.h"

#ifdef HAL_COMP_MODULE_ENABLED

/* Private Data -----------------------------------------------------*/

static const uint32_t PORT_MAP_PERIPHERAL_ID[CFN_HAL_COMP_PORT_MAX] = {
    [CFN_HAL_COMP_PORT_COMP] = CFN_HAL_PORT_PERIPH_COMP,
};

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t low_level_init(cfn_hal_comp_t *driver)
{
    if ((driver == NULL) || (driver->phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /* 1. Enable Clock */
    __HAL_RCC_COMP_CLK_ENABLE();

    /* 2. Initialize Pins */
    if (driver->phy->input_plus)
    {
        (void) cfn_hal_gpio_init(driver->phy->input_plus->port);
    }
    if (driver->phy->input_minus)
    {
        (void) cfn_hal_gpio_init(driver->phy->input_minus->port);
    }
    if (driver->phy->output)
    {
        (void) cfn_hal_gpio_init(driver->phy->output->port);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_comp_t *driver = (cfn_hal_comp_t *) base;
    if ((driver == NULL) || (driver->phy == NULL) || (driver->config == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    return low_level_init(driver);
}

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(config);
    return port_base_init(base);
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

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    CFN_HAL_UNUSED(base);
    if (error_mask)
    {
        *error_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

static const cfn_hal_comp_api_t COMP_API = {
    .base =
        {
            .init = NULL,
            .deinit = NULL,
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
    .read_output = NULL,
    .set_threshold = NULL,
    .start = NULL,
    .stop = NULL};

#endif /* HAL_COMP_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t cfn_hal_comp_construct(cfn_hal_comp_t              *driver,
                                            const cfn_hal_comp_config_t *config,
                                            const cfn_hal_comp_phy_t    *phy,
                                            struct cfn_hal_clock_s      *clock,
                                            cfn_hal_comp_callback_t      callback,
                                            void                        *user_arg)
{
#ifdef HAL_COMP_MODULE_ENABLED
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /* Port ID bounds checking */
    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_COMP_PORT_MAX)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t peripheral_id = PORT_MAP_PERIPHERAL_ID[port_id];

    cfn_hal_comp_populate(driver, peripheral_id, clock, &COMP_API, phy, config, callback, user_arg);

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

cfn_hal_error_code_t cfn_hal_comp_destruct(cfn_hal_comp_t *driver)
{
#ifdef HAL_COMP_MODULE_ENABLED
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->config = NULL;
    driver->phy    = NULL;
    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}
