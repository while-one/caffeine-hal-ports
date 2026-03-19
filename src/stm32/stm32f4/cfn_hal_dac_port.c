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

#ifdef HAL_DAC_MODULE_ENABLED

/* Private Data -----------------------------------------------------*/

static DAC_TypeDef *const PORT_INSTANCES[CFN_HAL_DAC_PORT_MAX] = {
#if defined(DAC)
    [CFN_HAL_DAC_PORT_DAC1] = DAC,
#endif
};

static DAC_HandleTypeDef port_hdacs[CFN_HAL_DAC_PORT_MAX];
static cfn_hal_dac_t    *port_drivers[CFN_HAL_DAC_PORT_MAX];

/* Internal Helpers -------------------------------------------------*/

static int32_t get_port_id_from_handle(DAC_HandleTypeDef *hdac)
{
    for (uint32_t i = 0; i < CFN_HAL_DAC_PORT_MAX; i++)
    {
        if (&port_hdacs[i] == hdac)
        {
            return (int32_t) i;
        }
    }
    return -1;
}

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

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(base);
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

static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_dac_t     *driver = (cfn_hal_dac_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    DAC_HandleTypeDef *hdac = &port_hdacs[port_id];

    if (error_mask & CFN_HAL_DAC_ERROR_UNDERRUN)
    {
        uint32_t channel = (driver->phy->channel == 1) ? DAC_CHANNEL_1 : DAC_CHANNEL_2;
        __HAL_DAC_ENABLE_IT(hdac, (channel == DAC_CHANNEL_1) ? DAC_IT_DMAUDR1 : DAC_IT_DMAUDR2);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_dac_t     *driver = (cfn_hal_dac_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    DAC_HandleTypeDef *hdac = &port_hdacs[port_id];

    if (error_mask & CFN_HAL_DAC_ERROR_UNDERRUN)
    {
        uint32_t channel = (driver->phy->channel == 1) ? DAC_CHANNEL_1 : DAC_CHANNEL_2;
        __HAL_DAC_DISABLE_IT(hdac, (channel == DAC_CHANNEL_1) ? DAC_IT_DMAUDR1 : DAC_IT_DMAUDR2);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    cfn_hal_dac_t     *driver = (cfn_hal_dac_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    DAC_HandleTypeDef *hdac = &port_hdacs[port_id];
    uint32_t           mask = 0;
    uint32_t           channel = (driver->phy->channel == 1) ? DAC_CHANNEL_1 : DAC_CHANNEL_2;

    if (__HAL_DAC_GET_FLAG(hdac, (channel == DAC_CHANNEL_1) ? DAC_FLAG_DMAUDR1 : DAC_FLAG_DMAUDR2))
    {
        mask |= CFN_HAL_DAC_ERROR_UNDERRUN;
    }

    if (error_mask != NULL)
    {
        *error_mask = mask;
    }
    return CFN_HAL_ERROR_OK;
}

/* ST HAL Callback Overrides ----------------------------------------*/

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
    int32_t port_id = get_port_id_from_handle(hdac);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_dac_t *driver = port_drivers[port_id];
        if ((driver->cb != NULL) && (driver->phy->channel == 1))
        {
            driver->cb(driver, CFN_HAL_DAC_EVENT_DMA_COMPLETE, 0, driver->cb_user_arg);
        }
    }
}

void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef *hdac)
{
    int32_t port_id = get_port_id_from_handle(hdac);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_dac_t *driver = port_drivers[port_id];
        if ((driver->cb != NULL) && (driver->phy->channel == 2))
        {
            driver->cb(driver, CFN_HAL_DAC_EVENT_DMA_COMPLETE, 0, driver->cb_user_arg);
        }
    }
}

void HAL_DAC_ErrorCallbackCh1(DAC_HandleTypeDef *hdac)
{
    int32_t port_id = get_port_id_from_handle(hdac);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_dac_t *driver = port_drivers[port_id];
        if ((driver->cb != NULL) && (driver->phy->channel == 1))
        {
            uint32_t error_mask = 0;
            (void) port_base_error_get(&driver->base, &error_mask);
            driver->cb(driver, CFN_HAL_DAC_EVENT_NONE, error_mask, driver->cb_user_arg);
        }
    }
}

void HAL_DACEx_ErrorCallbackCh2(DAC_HandleTypeDef *hdac)
{
    int32_t port_id = get_port_id_from_handle(hdac);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_dac_t *driver = port_drivers[port_id];
        if ((driver->cb != NULL) && (driver->phy->channel == 2))
        {
            uint32_t error_mask = 0;
            (void) port_base_error_get(&driver->base, &error_mask);
            driver->cb(driver, CFN_HAL_DAC_EVENT_NONE, error_mask, driver->cb_user_arg);
        }
    }
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
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    uint32_t channel = (driver->phy->channel == 1) ? DAC_CHANNEL_1 : DAC_CHANNEL_2;
    return cfn_hal_stm32_map_error(HAL_DAC_Start_DMA(
        &port_hdacs[port_id], channel, (uint32_t *) data, (uint32_t) nbr_of_samples, DAC_ALIGN_12B_R));
}

/* API --------------------------------------------------------------*/
static const cfn_hal_dac_api_t DAC_API = {
    .base = {
        .init = port_base_init,
        .deinit = port_base_deinit,
        .power_state_set = NULL,
        .config_set = port_base_config_set,
        .callback_register = NULL,
        .event_enable = NULL,
        .event_disable = NULL,
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

#endif /* HAL_DAC_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_dac_construct(cfn_hal_dac_t *driver, const cfn_hal_dac_config_t *config, const cfn_hal_dac_phy_t *phy)
{
#ifdef HAL_DAC_MODULE_ENABLED
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
    port_drivers[port_id] = driver;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    CFN_HAL_UNUSED(phy);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

cfn_hal_error_code_t cfn_hal_dac_destruct(cfn_hal_dac_t *driver)
{
#ifdef HAL_DAC_MODULE_ENABLED
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id < CFN_HAL_DAC_PORT_MAX)
    {
        port_drivers[port_id] = NULL;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_DAC;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}
