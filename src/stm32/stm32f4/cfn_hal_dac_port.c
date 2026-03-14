/**
 * @file cfn_hal_dac_port.c
 * @brief STM32F4 DAC HAL Port Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_dac.h"
#include "cfn_hal_dac_port.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_stm32_error.h"

/* Private Data -----------------------------------------------------*/

static DAC_TypeDef *const PORT_INSTANCES[CFN_HAL_DAC_PORT_MAX] = {
#if defined(DAC)
    [CFN_HAL_DAC_PORT_DAC1] = DAC,
#endif
};

static DAC_HandleTypeDef port_hdacs[CFN_HAL_DAC_PORT_MAX];

/* VMT Implementations ----------------------------------------------*/

static void low_level_init(cfn_hal_dac_t *driver)
{
    /* 1. Enable Clock */
    cfn_hal_port_clock_enable_gate(CFN_HAL_PORT_PERIPH_DAC1);

    /* 2. Initialize Pin (Mapped via DAC channel in STM32F4) */
    if (driver->phy->pin)
    {
        (void) cfn_hal_base_init((cfn_hal_driver_t *) driver->phy->pin, CFN_HAL_PERIPHERAL_TYPE_GPIO);
    }
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_dac_t     *driver = (cfn_hal_dac_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    DAC_HandleTypeDef *hdac = &port_hdacs[port_id];

    low_level_init(driver);

    hdac->Instance = PORT_INSTANCES[port_id];

    return cfn_hal_stm32_map_error(HAL_DAC_Init(hdac));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_dac_t *driver = (cfn_hal_dac_t *) base;
    uint32_t       port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_DAC_DeInit(&port_hdacs[port_id]));
}

/* ... base stubs ... */
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

/* DAC Specific Functions */

static cfn_hal_error_code_t port_dac_set_value(cfn_hal_dac_t *driver, uint32_t value)
{
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    DAC_HandleTypeDef *hdac = &port_hdacs[port_id];
    uint32_t           channel = (driver->phy->channel == 1) ? DAC_CHANNEL_1 : DAC_CHANNEL_2;

    return cfn_hal_stm32_map_error(HAL_DAC_SetValue(hdac, channel, DAC_ALIGN_12B_R, value));
}

static cfn_hal_error_code_t port_dac_start(cfn_hal_dac_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    uint32_t channel = (driver->phy->channel == 1) ? DAC_CHANNEL_1 : DAC_CHANNEL_2;
    return cfn_hal_stm32_map_error(HAL_DAC_Start(&port_hdacs[port_id], channel));
}

static cfn_hal_error_code_t port_dac_stop(cfn_hal_dac_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    uint32_t channel = (driver->phy->channel == 1) ? DAC_CHANNEL_1 : DAC_CHANNEL_2;
    return cfn_hal_stm32_map_error(HAL_DAC_Stop(&port_hdacs[port_id], channel));
}

static cfn_hal_error_code_t port_dac_write_dma(cfn_hal_dac_t *driver, const uint32_t *data, size_t nbr_of_samples)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(nbr_of_samples);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_dac_api_t DAC_API = {
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
    .set_value = port_dac_set_value,
    .start = port_dac_start,
    .stop = port_dac_stop,
    .write_dma = port_dac_write_dma
};

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_dac_construct(cfn_hal_dac_t *driver, const cfn_hal_dac_config_t *config, const cfn_hal_dac_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_DAC_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &DAC_API;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_DAC;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;

    port_hdacs[port_id].Instance = PORT_INSTANCES[port_id];

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_dac_destruct(cfn_hal_dac_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_DAC;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
}
