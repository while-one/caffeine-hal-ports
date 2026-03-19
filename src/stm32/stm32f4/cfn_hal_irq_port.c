/**
 * @file cfn_hal_irq_port.c
 * @brief STM32F4 IRQ HAL Port Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_irq.h"
#include "cfn_hal_irq_port.h"

/* VMT Implementations ----------------------------------------------*/

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
