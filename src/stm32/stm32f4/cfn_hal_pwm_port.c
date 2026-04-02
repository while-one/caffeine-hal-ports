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
 * @file cfn_hal_pwm_port.c
 * @brief STM32F4 PWM HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_pwm_port.h"
#include "cfn_hal_clock.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_pwm.h"
#include "cfn_hal_stm32_error.h"
#include "cfn_hal_timer_port.h"
#include "stm32f4xx_hal.h"

#ifdef HAL_TIM_MODULE_ENABLED

/* Private Data -----------------------------------------------------*/

/**
 * @brief Mapping from Caffeine Timer port IDs to global clock peripheral IDs.
 */
static const uint32_t PORT_MAP_PERIPHERAL_ID[CFN_HAL_TIMER_PORT_MAX] = {
    [CFN_HAL_TIMER_PORT_TIM1] = CFN_HAL_PORT_PERIPH_TIM1,   [CFN_HAL_TIMER_PORT_TIM2] = CFN_HAL_PORT_PERIPH_TIM2,
    [CFN_HAL_TIMER_PORT_TIM3] = CFN_HAL_PORT_PERIPH_TIM3,   [CFN_HAL_TIMER_PORT_TIM4] = CFN_HAL_PORT_PERIPH_TIM4,
    [CFN_HAL_TIMER_PORT_TIM5] = CFN_HAL_PORT_PERIPH_TIM5,   [CFN_HAL_TIMER_PORT_TIM6] = CFN_HAL_PORT_PERIPH_TIM6,
    [CFN_HAL_TIMER_PORT_TIM7] = CFN_HAL_PORT_PERIPH_TIM7,   [CFN_HAL_TIMER_PORT_TIM8] = CFN_HAL_PORT_PERIPH_TIM8,
    [CFN_HAL_TIMER_PORT_TIM9] = CFN_HAL_PORT_PERIPH_TIM9,   [CFN_HAL_TIMER_PORT_TIM10] = CFN_HAL_PORT_PERIPH_TIM10,
    [CFN_HAL_TIMER_PORT_TIM11] = CFN_HAL_PORT_PERIPH_TIM11, [CFN_HAL_TIMER_PORT_TIM12] = CFN_HAL_PORT_PERIPH_TIM12,
    [CFN_HAL_TIMER_PORT_TIM13] = CFN_HAL_PORT_PERIPH_TIM13, [CFN_HAL_TIMER_PORT_TIM14] = CFN_HAL_PORT_PERIPH_TIM14,
};

static TIM_TypeDef *const PORT_INSTANCES[CFN_HAL_TIMER_PORT_MAX] = {
#if defined(TIM1)
    [CFN_HAL_TIMER_PORT_TIM1] = TIM1,
#endif
#if defined(TIM2)
    [CFN_HAL_TIMER_PORT_TIM2] = TIM2,
#endif
#if defined(TIM3)
    [CFN_HAL_TIMER_PORT_TIM3] = TIM3,
#endif
#if defined(TIM4)
    [CFN_HAL_TIMER_PORT_TIM4] = TIM4,
#endif
#if defined(TIM5)
    [CFN_HAL_TIMER_PORT_TIM5] = TIM5,
#endif
#if defined(TIM6)
    [CFN_HAL_TIMER_PORT_TIM6] = TIM6,
#endif
#if defined(TIM7)
    [CFN_HAL_TIMER_PORT_TIM7] = TIM7,
#endif
#if defined(TIM8)
    [CFN_HAL_TIMER_PORT_TIM8] = TIM8,
#endif
#if defined(TIM9)
    [CFN_HAL_TIMER_PORT_TIM9] = TIM9,
#endif
#if defined(TIM10)
    [CFN_HAL_TIMER_PORT_TIM10] = TIM10,
#endif
#if defined(TIM11)
    [CFN_HAL_TIMER_PORT_TIM11] = TIM11,
#endif
#if defined(TIM12)
    [CFN_HAL_TIMER_PORT_TIM12] = TIM12,
#endif
#if defined(TIM13)
    [CFN_HAL_TIMER_PORT_TIM13] = TIM13,
#endif
#if defined(TIM14)
    [CFN_HAL_TIMER_PORT_TIM14] = TIM14,
#endif
};

static TIM_HandleTypeDef port_htims[CFN_HAL_TIMER_PORT_MAX];
static cfn_hal_pwm_t    *port_drivers[CFN_HAL_TIMER_PORT_MAX];

/* Internal Helpers -------------------------------------------------*/

static uint32_t get_hal_channel(uint32_t channel)
{
    switch (channel)
    {
        case 1:
            return TIM_CHANNEL_1;
        case 2:
            return TIM_CHANNEL_2;
        case 3:
            return TIM_CHANNEL_3;
        case 4:
            return TIM_CHANNEL_4;
        default:
            return TIM_CHANNEL_1;
    }
}

static uint32_t get_timer_clock(const TIM_TypeDef *instance)
{
    uint32_t ppre1 = (RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;
    uint32_t ppre2 = (RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos;

    /* On F4, if PPRE is not 1, timer clock is 2x peripheral clock */
    if (instance == TIM1 || instance == TIM8 || instance == TIM9 || instance == TIM10 || instance == TIM11)
    {
        uint32_t pclk2 = HAL_RCC_GetPCLK2Freq();
        return (ppre2 == RCC_HCLK_DIV1) ? pclk2 : (pclk2 * 2);
    }
    else
    {
        uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
        return (ppre1 == RCC_HCLK_DIV1) ? pclk1 : (pclk1 * 2);
    }
}

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t low_level_init(cfn_hal_pwm_t *driver)
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
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id >= CFN_HAL_TIMER_PORT_MAX)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /* 1. Enable Clock */
    cfn_hal_clock_enable_gate((cfn_hal_clock_t *) clk, driver->base.peripheral_id);

    /* 2. Initialize Pin */
    if (driver->phy->pin)
    {
        (void) cfn_hal_gpio_init(driver->phy->pin->port);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_pwm_t       *driver = (cfn_hal_pwm_t *) base;
    cfn_hal_error_code_t error;
    uint32_t             port_id     = (uint32_t) (uintptr_t) driver->phy->instance;
    TIM_HandleTypeDef   *htim        = &port_htims[port_id];
    TIM_OC_InitTypeDef   s_config_oc = { 0 };

    error                            = low_level_init(driver);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }

    uint32_t timer_clk = get_timer_clock(PORT_INSTANCES[port_id]);
    uint32_t prescaler = 0;
    uint32_t period    = 0;

    /* Target 1MHz internal resolution for easy calculation if possible */
    if (timer_clk >= 1000000)
    {
        prescaler = (timer_clk / 1000000) - 1;
        period    = (1000000 / driver->config->frequency_hz) - 1;
    }
    else
    {
        prescaler = 0;
        period    = (timer_clk / driver->config->frequency_hz) - 1;
    }

    htim->Instance               = PORT_INSTANCES[port_id];
    htim->Init.Prescaler         = prescaler;
    htim->Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim->Init.Period            = period;
    htim->Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim->Init.RepetitionCounter = 0;
    htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_PWM_Init(htim) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }

    s_config_oc.OCMode = TIM_OCMODE_PWM1;
    s_config_oc.Pulse  = (period * driver->config->duty_cycle_percent) / 100;
    s_config_oc.OCPolarity =
        (driver->config->polarity == CFN_HAL_PWM_CONFIG_POLARITY_NORMAL) ? TIM_OCPOLARITY_HIGH : TIM_OCPOLARITY_LOW;
    s_config_oc.OCFastMode = TIM_OCFAST_DISABLE;

    return cfn_hal_stm32_map_error(
        HAL_TIM_PWM_ConfigChannel(htim, &s_config_oc, get_hal_channel(driver->phy->channel)));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_pwm_t *driver  = (cfn_hal_pwm_t *) base;
    uint32_t       port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_TIM_PWM_DeInit(&port_htims[port_id]));
}

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(config);
    return port_base_init(base);
}

static cfn_hal_error_code_t port_base_event_enable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    cfn_hal_pwm_t     *driver  = (cfn_hal_pwm_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    TIM_HandleTypeDef *htim    = &port_htims[port_id];

    if (event_mask & CFN_HAL_PWM_EVENT_PERIOD_MATCH)
    {
        __HAL_TIM_ENABLE_IT(htim, TIM_IT_UPDATE);
    }
    if (event_mask & CFN_HAL_PWM_EVENT_PULSE_MATCH)
    {
        uint32_t channel = get_hal_channel(driver->phy->channel);
        __HAL_TIM_ENABLE_IT(htim,
                            (channel == TIM_CHANNEL_1)   ? TIM_IT_CC1
                            : (channel == TIM_CHANNEL_2) ? TIM_IT_CC2
                            : (channel == TIM_CHANNEL_3) ? TIM_IT_CC3
                                                         : TIM_IT_CC4);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    cfn_hal_pwm_t     *driver  = (cfn_hal_pwm_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    TIM_HandleTypeDef *htim    = &port_htims[port_id];

    if (event_mask & CFN_HAL_PWM_EVENT_PERIOD_MATCH)
    {
        __HAL_TIM_DISABLE_IT(htim, TIM_IT_UPDATE);
    }
    if (event_mask & CFN_HAL_PWM_EVENT_PULSE_MATCH)
    {
        uint32_t channel = get_hal_channel(driver->phy->channel);
        __HAL_TIM_DISABLE_IT(htim,
                             (channel == TIM_CHANNEL_1)   ? TIM_IT_CC1
                             : (channel == TIM_CHANNEL_2) ? TIM_IT_CC2
                             : (channel == TIM_CHANNEL_3) ? TIM_IT_CC3
                                                          : TIM_IT_CC4);
    }

    return CFN_HAL_ERROR_OK;
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

static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_pwm_t     *driver  = (cfn_hal_pwm_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    TIM_HandleTypeDef *htim    = &port_htims[port_id];

    if (error_mask & CFN_HAL_PWM_ERROR_FAULT)
    {
        __HAL_TIM_ENABLE_IT(htim, TIM_IT_BREAK);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_pwm_t     *driver  = (cfn_hal_pwm_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    TIM_HandleTypeDef *htim    = &port_htims[port_id];

    if (error_mask & CFN_HAL_PWM_ERROR_FAULT)
    {
        __HAL_TIM_DISABLE_IT(htim, TIM_IT_BREAK);
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

/* PWM Specific Functions */

static cfn_hal_error_code_t port_pwm_start(cfn_hal_pwm_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_TIM_PWM_Start(&port_htims[port_id], get_hal_channel(driver->phy->channel)));
}

static cfn_hal_error_code_t port_pwm_stop(cfn_hal_pwm_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_TIM_PWM_Stop(&port_htims[port_id], get_hal_channel(driver->phy->channel)));
}

static cfn_hal_error_code_t port_pwm_set_frequency(cfn_hal_pwm_t *driver, uint32_t frequency_hz)
{
    uint32_t           port_id   = (uint32_t) (uintptr_t) driver->phy->instance;
    TIM_HandleTypeDef *htim      = &port_htims[port_id];
    uint32_t           timer_clk = get_timer_clock(PORT_INSTANCES[port_id]);
    uint32_t           prescaler = 0;
    uint32_t           period    = 0;
    bool               is_32bit  = (PORT_INSTANCES[port_id] == TIM2 || PORT_INSTANCES[port_id] == TIM5);

    calculate_pwm_params(timer_clk, frequency_hz, is_32bit, &prescaler, &period);

    __HAL_TIM_SET_PRESCALER(htim, prescaler);
    __HAL_TIM_SET_AUTORELOAD(htim, period);

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_pwm_set_duty_cycle(cfn_hal_pwm_t *driver, uint32_t duty_percent)
{
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    TIM_HandleTypeDef *htim    = &port_htims[port_id];
    uint32_t           period  = __HAL_TIM_GET_AUTORELOAD(htim);
    uint32_t           pulse   = (period * duty_percent) / 100;

    __HAL_TIM_SET_COMPARE(htim, get_hal_channel(driver->phy->channel), pulse);

    return CFN_HAL_ERROR_OK;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_pwm_api_t PWM_API = {
    .base =
        {
            .init = port_base_init,
            .deinit = port_base_deinit,
            .power_state_set = NULL,
            .config_set = port_base_config_set,
            .config_validate = NULL,
            .callback_register = NULL,
            .event_enable = port_base_event_enable,
            .event_disable = port_base_event_disable,
            .event_get = port_base_event_get,
            .error_enable = port_base_error_enable,
            .error_disable = port_base_error_disable,
            .error_get = port_base_error_get,
        },
    .start = port_pwm_start,
    .stop = port_pwm_stop,
    .set_frequency = port_pwm_set_frequency,
    .set_duty_cycle = port_pwm_set_duty_cycle};

#endif /* HAL_TIM_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t cfn_hal_pwm_construct(cfn_hal_pwm_t              *driver,
                                           const cfn_hal_pwm_config_t *config,
                                           const cfn_hal_pwm_phy_t    *phy,
                                           struct cfn_hal_clock_s     *clock,
                                           cfn_hal_pwm_callback_t      callback,
                                           void                       *user_arg)
{
#ifdef HAL_TIM_MODULE_ENABLED
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_TIMER_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t peripheral_id = PORT_MAP_PERIPHERAL_ID[port_id];
    cfn_hal_pwm_populate(driver, peripheral_id, clock, &PWM_API, phy, config, callback, user_arg);

    port_htims[port_id].Instance = PORT_INSTANCES[port_id];
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

cfn_hal_error_code_t cfn_hal_pwm_destruct(cfn_hal_pwm_t *driver)
{
#ifdef HAL_TIM_MODULE_ENABLED
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (driver->phy != NULL)
    {
        uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
        if (port_id < CFN_HAL_TIMER_PORT_MAX)
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
