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
 * @file cfn_hal_timer_port.c
 * @brief STM32F4 TIMER HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_timer_port.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_stm32_error.h"
#include "cfn_hal_timer.h"
#include "stm32f4xx_hal.h"

#ifdef HAL_TIM_MODULE_ENABLED

/* Private Data -----------------------------------------------------*/

/**
 * @brief Mapping from Caffeine Timer port IDs to global clock peripheral IDs.
 */
static const cfn_hal_port_peripheral_id_t PORT_MAP_CLOCK_PERIPHERAL_ID[CFN_HAL_TIMER_PORT_MAX] = {
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
static cfn_hal_timer_t  *port_drivers[CFN_HAL_TIMER_PORT_MAX];

/* Internal Helpers -------------------------------------------------*/

static uint32_t get_port_id_from_handle(TIM_HandleTypeDef *handle)
{
    if ((handle < &port_htims[0]) || (handle >= &port_htims[CFN_HAL_TIMER_PORT_MAX]))
    {
        return UINT32_MAX;
    }
    return (uint32_t) (handle - port_htims);
}

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t low_level_init(cfn_hal_timer_t *driver)
{
    if ((driver == NULL) || (driver->phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id >= CFN_HAL_TIMER_PORT_MAX)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /* 1. Enable Clock */
    cfn_hal_port_clock_enable_gate(PORT_MAP_CLOCK_PERIPHERAL_ID[port_id]);

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_timer_t *driver = (cfn_hal_timer_t *) base;

    error                   = low_level_init(driver);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }

    uint32_t           port_id   = (uint32_t) (uintptr_t) driver->phy->instance;
    TIM_HandleTypeDef *htim      = &port_htims[port_id];

    htim->Instance               = PORT_INSTANCES[port_id];
    htim->Init.Prescaler         = driver->config->prescaler;
    htim->Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim->Init.Period            = driver->config->period;
    htim->Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim->Init.RepetitionCounter = 0;
    htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    return cfn_hal_stm32_map_error(HAL_TIM_Base_Init(htim));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_timer_t *driver  = (cfn_hal_timer_t *) base;
    uint32_t         port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_TIM_Base_DeInit(&port_htims[port_id]));
}

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(config);
    return port_base_init(base);
}

static cfn_hal_error_code_t port_base_event_enable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    cfn_hal_timer_t   *driver  = (cfn_hal_timer_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    TIM_HandleTypeDef *htim    = &port_htims[port_id];

    if (event_mask & CFN_HAL_TIMER_EVENT_UPDATE)
    {
        __HAL_TIM_ENABLE_IT(htim, TIM_IT_UPDATE);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    cfn_hal_timer_t   *driver  = (cfn_hal_timer_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    TIM_HandleTypeDef *htim    = &port_htims[port_id];

    if (event_mask & CFN_HAL_TIMER_EVENT_UPDATE)
    {
        __HAL_TIM_DISABLE_IT(htim, TIM_IT_UPDATE);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask)
{
    cfn_hal_timer_t   *driver  = (cfn_hal_timer_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    TIM_HandleTypeDef *htim    = &port_htims[port_id];
    uint32_t           mask    = 0;

    if (__HAL_TIM_GET_FLAG(htim, TIM_FLAG_UPDATE))
    {
        mask |= CFN_HAL_TIMER_EVENT_UPDATE;
    }

    if (event_mask != NULL)
    {
        *event_mask = mask;
    }
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_timer_t   *driver  = (cfn_hal_timer_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    TIM_HandleTypeDef *htim    = &port_htims[port_id];

    if (error_mask & CFN_HAL_TIMER_ERROR_BREAK)
    {
        __HAL_TIM_ENABLE_IT(htim, TIM_IT_BREAK);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_timer_t   *driver  = (cfn_hal_timer_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    TIM_HandleTypeDef *htim    = &port_htims[port_id];

    if (error_mask & CFN_HAL_TIMER_ERROR_BREAK)
    {
        __HAL_TIM_DISABLE_IT(htim, TIM_IT_BREAK);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    cfn_hal_timer_t   *driver  = (cfn_hal_timer_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    TIM_HandleTypeDef *htim    = &port_htims[port_id];
    uint32_t           mask    = 0;

    if (__HAL_TIM_GET_FLAG(htim, TIM_FLAG_BREAK))
    {
        mask |= CFN_HAL_TIMER_ERROR_BREAK;
    }

    if (error_mask != NULL)
    {
        *error_mask = mask;
    }
    return CFN_HAL_ERROR_OK;
}

/* ST HAL Callback Overrides ----------------------------------------*/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    uint32_t port_id = get_port_id_from_handle(htim);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_timer_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_TIMER_EVENT_UPDATE, CFN_HAL_TIMER_ERROR_NONE, 0, driver->cb_user_arg);
        }
    }
}

void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim)
{
    uint32_t port_id = get_port_id_from_handle(htim);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_timer_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            uint32_t error_mask = 0;
            (void) port_base_error_get(&driver->base, &error_mask);
            driver->cb(driver, CFN_HAL_TIMER_EVENT_NONE, error_mask, 0, driver->cb_user_arg);
        }
    }
}

/* Raw ISR Handlers -------------------------------------------------*/

#ifndef CFN_HAL_PORT_DISABLE_IRQ_TIMER

#if defined(TIM1)
void TIM1_UP_TIM10_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM1]);
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM10]);
}
void TIM1_BRK_TIM9_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM1]);
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM9]);
}
void TIM1_TRG_COM_TIM11_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM1]);
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM11]);
}
#endif

#if defined(TIM2)
void TIM2_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM2]);
}
#endif
#if defined(TIM3)
void TIM3_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM3]);
}
#endif
#if defined(TIM4)
void TIM4_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM4]);
}
#endif
#if defined(TIM5)
void TIM5_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM5]);
}
#endif
#if defined(TIM6)
void TIM6_DAC_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM6]);
}
#endif
#if defined(TIM7)
void TIM7_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM7]);
}
#endif
#if defined(TIM8)
void TIM8_UP_TIM13_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM8]);
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM13]);
}
void TIM8_BRK_TIM12_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM8]);
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM12]);
}
void TIM8_TRG_COM_TIM14_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM8]);
    HAL_TIM_IRQHandler(&port_htims[CFN_HAL_TIMER_PORT_TIM14]);
}
#endif

#endif /* CFN_HAL_PORT_DISABLE_IRQ_TIMER */

/* Timer Specific Functions */

static cfn_hal_error_code_t port_timer_start(cfn_hal_timer_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_TIM_Base_Start_IT(&port_htims[port_id]));
}

static cfn_hal_error_code_t port_timer_stop(cfn_hal_timer_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_TIM_Base_Stop_IT(&port_htims[port_id]));
}

static cfn_hal_error_code_t port_timer_get_ticks(cfn_hal_timer_t *driver, uint32_t ch, uint32_t *ticks)
{
    CFN_HAL_UNUSED(ch);
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (!ticks)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *ticks = __HAL_TIM_GET_COUNTER(&port_htims[port_id]);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_timer_get_ticks_u64(cfn_hal_timer_t *driver, uint32_t ch, uint64_t *ticks)
{
    uint32_t             t32   = 0;
    cfn_hal_error_code_t error = port_timer_get_ticks(driver, ch, &t32);
    if (error == CFN_HAL_ERROR_OK)
    {
        *ticks = (uint64_t) t32;
    }
    return error;
}

static cfn_hal_error_code_t port_timer_set_period(cfn_hal_timer_t *driver, const cfn_hal_timer_period_t *period)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    __HAL_TIM_SET_AUTORELOAD(&port_htims[port_id], (uint32_t) period->ticks);
    return CFN_HAL_ERROR_OK;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_timer_api_t TIMER_API = {
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
    .start = port_timer_start,
    .stop = port_timer_stop,
    .get_ticks = port_timer_get_ticks,
    .get_ticks_u64 = port_timer_get_ticks_u64,
    .set_period = port_timer_set_period};

#endif /* HAL_TIM_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_timer_construct(cfn_hal_timer_t *driver, const cfn_hal_timer_config_t *config, const cfn_hal_timer_phy_t *phy)
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

    driver->api                  = &TIMER_API;
    driver->base.type            = CFN_HAL_PERIPHERAL_TYPE_TIMER;
    driver->base.status          = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config               = config;
    driver->phy                  = phy;

    port_htims[port_id].Instance = PORT_INSTANCES[port_id];
    port_drivers[port_id]        = driver;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    CFN_HAL_UNUSED(phy);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

cfn_hal_error_code_t cfn_hal_timer_destruct(cfn_hal_timer_t *driver)
{
#ifdef HAL_TIM_MODULE_ENABLED
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id < CFN_HAL_TIMER_PORT_MAX)
    {
        port_drivers[port_id] = NULL;
    }

    driver->api         = NULL;
    driver->base.type   = CFN_HAL_PERIPHERAL_TYPE_TIMER;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config      = NULL;
    driver->phy         = NULL;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}
