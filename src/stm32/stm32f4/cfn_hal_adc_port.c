/**
 * @file cfn_hal_adc_port.c
 * @brief STM32F4 ADC HAL Port Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_adc.h"
#include "cfn_hal_adc_port.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_stm32_error.h"

/* Private Data -----------------------------------------------------*/

static ADC_TypeDef *const PORT_INSTANCES[CFN_HAL_ADC_PORT_MAX] = {
#if defined(ADC1)
    [CFN_HAL_ADC_PORT_ADC1] = ADC1,
#endif
#if defined(ADC2)
    [CFN_HAL_ADC_PORT_ADC2] = ADC2,
#endif
#if defined(ADC3)
    [CFN_HAL_ADC_PORT_ADC3] = ADC3,
#endif
};

static ADC_HandleTypeDef port_hadcs[CFN_HAL_ADC_PORT_MAX];
static cfn_hal_adc_t    *port_drivers[CFN_HAL_ADC_PORT_MAX];

/* Internal Helpers -------------------------------------------------*/

static int32_t get_port_id_from_handle(ADC_HandleTypeDef *hadc)
{
    for (uint32_t i = 0; i < CFN_HAL_ADC_PORT_MAX; i++)
    {
        if (&port_hadcs[i] == hadc)
        {
            return (int32_t) i;
        }
    }
    return -1;
}

/* VMT Implementations ----------------------------------------------*/

static void low_level_init(cfn_hal_adc_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    /* 1. Enable Clock */
    cfn_hal_port_clock_enable_gate((cfn_hal_port_peripheral_id_t) (CFN_HAL_PORT_PERIPH_ADC1 + port_id));
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_adc_t     *driver = (cfn_hal_adc_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ADC_HandleTypeDef *hadc = &port_hadcs[port_id];

    low_level_init(driver);

    hadc->Instance = PORT_INSTANCES[port_id];
    hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc->Init.Resolution = ADC_RESOLUTION_12B;
    hadc->Init.ScanConvMode = DISABLE;
    hadc->Init.ContinuousConvMode = DISABLE;
    hadc->Init.DiscontinuousConvMode = DISABLE;
    hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc->Init.NbrOfConversion = 1;
    hadc->Init.DMAContinuousRequests = DISABLE;
    hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;

    return cfn_hal_stm32_map_error(HAL_ADC_Init(hadc));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_adc_t *driver = (cfn_hal_adc_t *) base;
    uint32_t       port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_ADC_DeInit(&port_hadcs[port_id]));
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
    cfn_hal_adc_t     *driver = (cfn_hal_adc_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ADC_HandleTypeDef *hadc = &port_hadcs[port_id];

    if (event_mask & CFN_HAL_ADC_EVENT_EOC)
    {
        __HAL_ADC_ENABLE_IT(hadc, ADC_IT_EOC);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    cfn_hal_adc_t     *driver = (cfn_hal_adc_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ADC_HandleTypeDef *hadc = &port_hadcs[port_id];

    if (event_mask & CFN_HAL_ADC_EVENT_EOC)
    {
        __HAL_ADC_DISABLE_IT(hadc, ADC_IT_EOC);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask)
{
    cfn_hal_adc_t     *driver = (cfn_hal_adc_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ADC_HandleTypeDef *hadc = &port_hadcs[port_id];
    uint32_t           mask = 0;

    if (__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_EOC))
    {
        mask |= CFN_HAL_ADC_EVENT_EOC;
    }

    if (event_mask != NULL)
    {
        *event_mask = mask;
    }
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_adc_t     *driver = (cfn_hal_adc_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ADC_HandleTypeDef *hadc = &port_hadcs[port_id];

    if (error_mask & CFN_HAL_ADC_ERROR_OVERRUN)
    {
        __HAL_ADC_ENABLE_IT(hadc, ADC_IT_OVR);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_adc_t     *driver = (cfn_hal_adc_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ADC_HandleTypeDef *hadc = &port_hadcs[port_id];

    if (error_mask & CFN_HAL_ADC_ERROR_OVERRUN)
    {
        __HAL_ADC_DISABLE_IT(hadc, ADC_IT_OVR);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    cfn_hal_adc_t     *driver = (cfn_hal_adc_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ADC_HandleTypeDef *hadc = &port_hadcs[port_id];
    uint32_t           mask = 0;

    if (__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_OVR))
    {
        mask |= CFN_HAL_ADC_ERROR_OVERRUN;
    }

    if (error_mask != NULL)
    {
        *error_mask = mask;
    }
    return CFN_HAL_ERROR_OK;
}

/* ST HAL Callback Overrides ----------------------------------------*/

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    int32_t port_id = get_port_id_from_handle(hadc);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_adc_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            uint32_t val = HAL_ADC_GetValue(hadc);
            driver->cb(driver, CFN_HAL_ADC_EVENT_EOC, CFN_HAL_ADC_ERROR_NONE, val, driver->cb_user_arg);
        }
    }
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
    int32_t port_id = get_port_id_from_handle(hadc);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_adc_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            uint32_t error_mask = 0;
            (void) port_base_error_get(&driver->base, &error_mask);
            driver->cb(driver, CFN_HAL_ADC_EVENT_NONE, error_mask, 0, driver->cb_user_arg);
        }
    }
}

/* Raw ISR Handlers -------------------------------------------------*/

#if defined(ADC1) || defined(ADC2) || defined(ADC3)
void ADC_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
#if defined(ADC1)
    HAL_ADC_IRQHandler(&port_hadcs[CFN_HAL_ADC_PORT_ADC1]);
#endif
#if defined(ADC2)
    HAL_ADC_IRQHandler(&port_hadcs[CFN_HAL_ADC_PORT_ADC2]);
#endif
#if defined(ADC3)
    HAL_ADC_IRQHandler(&port_hadcs[CFN_HAL_ADC_PORT_ADC3]);
#endif
}
#endif

/* ADC Specific Functions */

static cfn_hal_error_code_t port_adc_read(cfn_hal_adc_t *driver, uint32_t *data, uint32_t timeout)
{
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ADC_HandleTypeDef *hadc = &port_hadcs[port_id];

    HAL_StatusTypeDef status = HAL_ADC_Start(hadc);
    if (status != HAL_OK)
    {
        return cfn_hal_stm32_map_error(status);
    }

    status = HAL_ADC_PollForConversion(hadc, timeout);
    if (status != HAL_OK)
    {
        HAL_ADC_Stop(hadc);
        return cfn_hal_stm32_map_error(status);
    }

    *data = HAL_ADC_GetValue(hadc);
    HAL_ADC_Stop(hadc);

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_adc_start(cfn_hal_adc_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}
static cfn_hal_error_code_t port_adc_stop(cfn_hal_adc_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}
static cfn_hal_error_code_t port_adc_read_dma(cfn_hal_adc_t *driver, uint32_t *data, size_t nbr_of_samples)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(nbr_of_samples);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_adc_api_t ADC_API = {
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
    .read = port_adc_read,
    .start = port_adc_start,
    .stop = port_adc_stop,
    .read_dma = port_adc_read_dma
};

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_adc_construct(cfn_hal_adc_t *driver, const cfn_hal_adc_config_t *config, const cfn_hal_adc_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_ADC_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &ADC_API;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_ADC;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;

    port_hadcs[port_id].Instance = PORT_INSTANCES[port_id];
    port_drivers[port_id] = driver;

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_adc_destruct(cfn_hal_adc_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id < CFN_HAL_ADC_PORT_MAX)
    {
        port_drivers[port_id] = NULL;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_ADC;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
}
