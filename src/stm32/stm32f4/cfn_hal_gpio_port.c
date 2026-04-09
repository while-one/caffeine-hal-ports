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
 * @brief STM32F4 GPIO HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_gpio_port.h"
#include "cfn_hal_clock.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_stm32_error.h"
#include "stm32f4xx_hal.h"

/* Private Data -----------------------------------------------------*/

static GPIO_TypeDef *const PORT_INSTANCES[CFN_HAL_GPIO_PORT_MAX] = {
#if defined(GPIOA)
    [CFN_HAL_GPIO_PORT_A] = GPIOA,
#endif
#if defined(GPIOB)
    [CFN_HAL_GPIO_PORT_B] = GPIOB,
#endif
#if defined(GPIOC)
    [CFN_HAL_GPIO_PORT_C] = GPIOC,
#endif
#if defined(GPIOD)
    [CFN_HAL_GPIO_PORT_D] = GPIOD,
#endif
#if defined(GPIOE)
    [CFN_HAL_GPIO_PORT_E] = GPIOE,
#endif
#if defined(GPIOF)
    [CFN_HAL_GPIO_PORT_F] = GPIOF,
#endif
#if defined(GPIOG)
    [CFN_HAL_GPIO_PORT_G] = GPIOG,
#endif
#if defined(GPIOH)
    [CFN_HAL_GPIO_PORT_H] = GPIOH,
#endif
#if defined(GPIOI)
    [CFN_HAL_GPIO_PORT_I] = GPIOI,
#endif
};

static const uint32_t PORT_MAP_PERIPHERAL_ID[CFN_HAL_GPIO_PORT_MAX] = {
    [CFN_HAL_GPIO_PORT_A] = CFN_HAL_PORT_PERIPH_GPIOA, //
    [CFN_HAL_GPIO_PORT_B] = CFN_HAL_PORT_PERIPH_GPIOB, //
    [CFN_HAL_GPIO_PORT_C] = CFN_HAL_PORT_PERIPH_GPIOC, //
    [CFN_HAL_GPIO_PORT_D] = CFN_HAL_PORT_PERIPH_GPIOD, //
    [CFN_HAL_GPIO_PORT_E] = CFN_HAL_PORT_PERIPH_GPIOE, //
    [CFN_HAL_GPIO_PORT_F] = CFN_HAL_PORT_PERIPH_GPIOF, //
    [CFN_HAL_GPIO_PORT_G] = CFN_HAL_PORT_PERIPH_GPIOG, //
    [CFN_HAL_GPIO_PORT_H] = CFN_HAL_PORT_PERIPH_GPIOH, //
    [CFN_HAL_GPIO_PORT_I] = CFN_HAL_PORT_PERIPH_GPIOI, //
    [CFN_HAL_GPIO_PORT_J] = CFN_HAL_PORT_PERIPH_GPIOJ, //
    [CFN_HAL_GPIO_PORT_K] = CFN_HAL_PORT_PERIPH_GPIOK, //
};

static const uint32_t PORT_MAP_MODE[] = {
    [CFN_HAL_GPIO_CONFIG_MODE_INPUT]        = GPIO_MODE_INPUT,
    [CFN_HAL_GPIO_CONFIG_MODE_OUTPUT_PP]    = GPIO_MODE_OUTPUT_PP,
    [CFN_HAL_GPIO_CONFIG_MODE_OUTPUT_OD]    = GPIO_MODE_OUTPUT_OD,
    [CFN_HAL_GPIO_CONFIG_MODE_ALTERNATE_PP] = GPIO_MODE_AF_PP,
    [CFN_HAL_GPIO_CONFIG_MODE_ALTERNATE_OD] = GPIO_MODE_AF_OD,
    [CFN_HAL_GPIO_CONFIG_MODE_ANALOG]       = GPIO_MODE_ANALOG,
};

static const uint32_t PORT_MAP_PULL[] = {
    [CFN_HAL_GPIO_CONFIG_PULL_NOPULL]   = GPIO_NOPULL,
    [CFN_HAL_GPIO_CONFIG_PULL_PULLUP]   = GPIO_PULLUP,
    [CFN_HAL_GPIO_CONFIG_PULL_PULLDOWN] = GPIO_PULLDOWN,
};

static const uint32_t PORT_MAP_SPEED[] = {
    [CFN_HAL_GPIO_CONFIG_SPEED_LOW]       = GPIO_SPEED_FREQ_LOW,
    [CFN_HAL_GPIO_CONFIG_SPEED_MEDIUM]    = GPIO_SPEED_FREQ_MEDIUM,
    [CFN_HAL_GPIO_CONFIG_SPEED_HIGH]      = GPIO_SPEED_FREQ_HIGH,
    [CFN_HAL_GPIO_CONFIG_SPEED_VERY_HIGH] = GPIO_SPEED_FREQ_VERY_HIGH,
};

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t low_level_init(cfn_hal_gpio_t *driver)
{
    if (driver == NULL || driver->phy == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    struct cfn_hal_clock_s *clk = driver->base.clock_driver;
    if (clk == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->port;
    if (port_id >= CFN_HAL_GPIO_PORT_MAX)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /* 1. Enable Clock */
    cfn_hal_clock_enable_gate((cfn_hal_clock_t *) clk, driver->base.peripheral_id);

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_gpio_t *driver = (cfn_hal_gpio_t *) base;

    return low_level_init(driver);
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_gpio_t *driver  = (cfn_hal_gpio_t *) base;
    uint32_t        port_id = (uint32_t) (uintptr_t) driver->phy->port;
    HAL_GPIO_DeInit(PORT_INSTANCES[port_id], GPIO_PIN_All);
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

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    CFN_HAL_UNUSED(base);
    if (error_mask)
    {
        *error_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

/* GPIO Specific Functions */

static cfn_hal_error_code_t port_gpio_pin_config(cfn_hal_gpio_t *port, const cfn_hal_gpio_pin_config_t *pin_cfg)
{
    uint32_t         port_id = (uint32_t) (uintptr_t) port->phy->port;
    GPIO_InitTypeDef st_cfg  = { 0 };

    st_cfg.Pin               = pin_cfg->pin_mask;
    st_cfg.Mode              = PORT_MAP_MODE[pin_cfg->mode];
    st_cfg.Pull              = PORT_MAP_PULL[pin_cfg->pull];
    st_cfg.Speed             = PORT_MAP_SPEED[pin_cfg->speed];

    if ((pin_cfg->mode == CFN_HAL_GPIO_CONFIG_MODE_ALTERNATE_PP) ||
        (pin_cfg->mode == CFN_HAL_GPIO_CONFIG_MODE_ALTERNATE_OD))
    {
        st_cfg.Alternate = (uint32_t) (uintptr_t) pin_cfg->alternate;
    }

    HAL_GPIO_Init(PORT_INSTANCES[port_id], &st_cfg);

    if ((pin_cfg->mode == CFN_HAL_GPIO_CONFIG_MODE_OUTPUT_PP) || (pin_cfg->mode == CFN_HAL_GPIO_CONFIG_MODE_OUTPUT_OD))
    {
        HAL_GPIO_WritePin(PORT_INSTANCES[port_id],
                          pin_cfg->pin_mask,
                          (pin_cfg->default_state == CFN_HAL_GPIO_STATE_HIGH) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_gpio_pin_write(cfn_hal_gpio_t *port, cfn_hal_gpio_pin_t pin, cfn_hal_gpio_state_t state)
{
    uint32_t port_id = (uint32_t) (uintptr_t) port->phy->port;
    HAL_GPIO_WritePin(PORT_INSTANCES[port_id], pin, (state == CFN_HAL_GPIO_STATE_HIGH) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_gpio_pin_read(cfn_hal_gpio_t *port, cfn_hal_gpio_pin_t pin, cfn_hal_gpio_state_t *state)
{
    uint32_t port_id = (uint32_t) (uintptr_t) port->phy->port;
    if (!state)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *state = (HAL_GPIO_ReadPin(PORT_INSTANCES[port_id], pin) == GPIO_PIN_SET) ? CFN_HAL_GPIO_STATE_HIGH
                                                                              : CFN_HAL_GPIO_STATE_LOW;
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_gpio_pin_toggle(cfn_hal_gpio_t *port, cfn_hal_gpio_pin_t pin)
{
    uint32_t port_id = (uint32_t) (uintptr_t) port->phy->port;
    HAL_GPIO_TogglePin(PORT_INSTANCES[port_id], pin);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_gpio_port_write(cfn_hal_gpio_t *port, uint32_t pin_mask, uint32_t port_value)
{
    uint32_t port_id = (uint32_t) (uintptr_t) port->phy->port;
    MODIFY_REG(PORT_INSTANCES[port_id]->ODR, pin_mask, port_value & pin_mask);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_gpio_port_read(cfn_hal_gpio_t *port, uint32_t *port_value)
{
    uint32_t port_id = (uint32_t) (uintptr_t) port->phy->port;
    if (!port_value)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *port_value = PORT_INSTANCES[port_id]->IDR;
    return CFN_HAL_ERROR_OK;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_gpio_api_t GPIO_API = {
    .base =
        {
            .init = port_base_init,
            .deinit = port_base_deinit,
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
    .pin_config = port_gpio_pin_config,
    .pin_read = port_gpio_pin_read,
    .pin_write = port_gpio_pin_write,
    .pin_toggle = port_gpio_pin_toggle,
    .port_read = port_gpio_port_read,
    .port_write = port_gpio_port_write};

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t cfn_hal_gpio_construct(cfn_hal_gpio_t           *driver,
                                            const cfn_hal_gpio_phy_t *phy,
                                            struct cfn_hal_clock_s   *clock,
                                            void                     *dependency,
                                            cfn_hal_gpio_callback_t   callback,
                                            void                     *user_arg)
{
#ifdef HAL_GPIO_MODULE_ENABLED
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->port;
    if (port_id >= CFN_HAL_GPIO_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t peripheral_id = PORT_MAP_PERIPHERAL_ID[port_id];
    cfn_hal_gpio_populate(driver, peripheral_id, clock, dependency, &GPIO_API, phy, callback, user_arg);

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(phy);
    CFN_HAL_UNUSED(clock);
    CFN_HAL_UNUSED(dependency);
    CFN_HAL_UNUSED(callback);
    CFN_HAL_UNUSED(user_arg);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

cfn_hal_error_code_t cfn_hal_gpio_destruct(cfn_hal_gpio_t *driver)
{
#ifdef HAL_GPIO_MODULE_ENABLED
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_gpio_populate(driver, 0, NULL, NULL, NULL, NULL, NULL, NULL);
    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}