/**
 * @file cfn_hal_irq_port.c
 * @brief STM32F4 IRQ HAL Port Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_irq.h"
#include "cfn_hal_irq_port.h"

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
    if (event_mask != NULL)
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
    if (error_mask != NULL)
    {
        *error_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_irq_global_enable(cfn_hal_irq_t *driver)
{
    CFN_HAL_UNUSED(driver);
    __enable_irq();
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_irq_global_disable(cfn_hal_irq_t *driver)
{
    CFN_HAL_UNUSED(driver);
    __disable_irq();
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_irq_enable_vector(cfn_hal_irq_t *driver, uint32_t irq_id)
{
    CFN_HAL_UNUSED(driver);
    HAL_NVIC_EnableIRQ((IRQn_Type) irq_id);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_irq_disable_vector(cfn_hal_irq_t *driver, uint32_t irq_id)
{
    CFN_HAL_UNUSED(driver);
    HAL_NVIC_DisableIRQ((IRQn_Type) irq_id);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_irq_set_priority(cfn_hal_irq_t *driver, uint32_t irq_id, uint32_t priority)
{
    CFN_HAL_UNUSED(driver);
    HAL_NVIC_SetPriority((IRQn_Type) irq_id, priority, 0);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_irq_clear_pending(cfn_hal_irq_t *driver, uint32_t irq_id)
{
    CFN_HAL_UNUSED(driver);
    HAL_NVIC_ClearPendingIRQ((IRQn_Type) irq_id);
    return CFN_HAL_ERROR_OK;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_irq_api_t IRQ_API = {
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
    .global_enable = port_irq_global_enable,
    .global_disable = port_irq_global_disable,
    .enable_vector = port_irq_enable_vector,
    .disable_vector = port_irq_disable_vector,
    .set_priority = port_irq_set_priority,
    .clear_pending = port_irq_clear_pending
};

/* Instantiation ----------------------------------------------------*/
cfn_hal_error_code_t
cfn_hal_irq_construct(cfn_hal_irq_t *driver, const cfn_hal_irq_config_t *config, const cfn_hal_irq_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &IRQ_API;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_IRQ;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_irq_destruct(cfn_hal_irq_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_IRQ;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
}
