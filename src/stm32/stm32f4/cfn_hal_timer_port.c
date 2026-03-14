/**
 * @file cfn_hal_timer_port.c
 * @brief STM32F4 Timer HAL Port Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_timer.h"
#include "cfn_hal_timer_port.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_stm32_error.h"

/* Private Data -----------------------------------------------------*/

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

/* VMT Implementations ----------------------------------------------*/

static void low_level_init(cfn_hal_timer_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    /* 1. Enable Clock */
    cfn_hal_port_clock_enable_gate((cfn_hal_port_peripheral_id_t) (CFN_HAL_PORT_PERIPH_TIM1 + port_id));
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_timer_t   *driver = (cfn_hal_timer_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    TIM_HandleTypeDef *htim = &port_htims[port_id];

    low_level_init(driver);

    htim->Instance = PORT_INSTANCES[port_id];
    htim->Init.Prescaler = driver->config->prescaler;
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim->Init.Period = driver->config->period;
    htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim->Init.RepetitionCounter = 0;
    htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    return cfn_hal_stm32_map_error(HAL_TIM_Base_Init(htim));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_timer_t *driver = (cfn_hal_timer_t *) base;
    uint32_t         port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_TIM_Base_DeInit(&port_htims[port_id]));
}

/* ... standard base stubs ... */
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
    return port_base_init(base);
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

/* Timer Specific Functions */

static cfn_hal_error_code_t port_timer_start(cfn_hal_timer_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_TIM_Base_Start(&port_htims[port_id]));
}

static cfn_hal_error_code_t port_timer_stop(cfn_hal_timer_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_TIM_Base_Stop(&port_htims[port_id]));
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
    uint32_t             t32 = 0;
    cfn_hal_error_code_t err = port_timer_get_ticks(driver, ch, &t32);
    if (err == CFN_HAL_ERROR_OK)
    {
        *ticks = (uint64_t) t32;
    }
    return err;
}

static cfn_hal_error_code_t port_timer_set_period(cfn_hal_timer_t *driver, const cfn_hal_timer_period_t *period)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    __HAL_TIM_SET_AUTORELOAD(&port_htims[port_id], (uint32_t) period->ticks);
    return CFN_HAL_ERROR_OK;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_timer_api_t TIMER_API = {
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
    },
    .start = port_timer_start,
    .stop = port_timer_stop,
    .get_ticks = port_timer_get_ticks,
    .get_ticks_u64 = port_timer_get_ticks_u64,
    .set_period = port_timer_set_period
};

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_timer_construct(cfn_hal_timer_t *driver, const cfn_hal_timer_config_t *config, const cfn_hal_timer_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_TIMER_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &TIMER_API;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_TIMER;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;

    port_htims[port_id].Instance = PORT_INSTANCES[port_id];

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_timer_destruct(cfn_hal_timer_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_TIMER;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
}
