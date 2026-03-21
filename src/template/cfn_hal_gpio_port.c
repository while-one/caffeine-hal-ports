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
 * @file cfn_hal_gpio_port.c
 * @brief Generic GPIO HAL Port Template Implementation
 */

#include "cfn_hal_gpio.h"
#include "cfn_hal_gpio_port.h"

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask) { CFN_HAL_UNUSED(base); if (event_mask) { *event_mask = 0; } return CFN_HAL_ERROR_OK; }

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask) { CFN_HAL_UNUSED(base); if (error_mask) { *error_mask = 0; } return CFN_HAL_ERROR_OK; }

static const cfn_hal_gpio_api_t gpio_api = {
    .base = {
        .init = NULL,
        .deinit = NULL,
        .power_state_set = NULL,
        .config_set = NULL,
        .callback_register = NULL,
        .event_enable = NULL,
        .event_disable = NULL,
        .event_get = port_base_event_get,
        .error_enable = NULL,
        .error_disable = NULL,
        .error_get = port_base_error_get,
    },
    .pin_config = NULL,
    .pin_read = NULL,
    .pin_write = NULL,
    .pin_toggle = NULL,
    .port_read = NULL,
    .port_write = NULL
};

cfn_hal_error_code_t cfn_hal_gpio_construct(cfn_hal_gpio_t           *driver,
                                            const cfn_hal_gpio_phy_t *phy,
                                            struct cfn_hal_clock_s   *clock,
                                            cfn_hal_gpio_callback_t   callback,
                                            void                     *user_arg)
{
    if ((driver == NULL) || (phy == NULL)) { return CFN_HAL_ERROR_BAD_PARAM; }
    cfn_hal_gpio_populate(driver, 0, clock, &gpio_api, phy, callback, user_arg);
    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_gpio_destruct(cfn_hal_gpio_t *driver)
{
    if (driver == NULL) { return CFN_HAL_ERROR_BAD_PARAM; }
    cfn_hal_gpio_populate(driver, 0, NULL, NULL, NULL, NULL, NULL);
    return CFN_HAL_ERROR_OK;
}
