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
 * @file cfn_hal_adc_port.c
 * @brief STM32F4 ADC HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_adc.h"
#include "cfn_hal_adc_port.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_stm32_error.h"

#ifdef HAL_ADC_MODULE_ENABLED

/* Private Data -----------------------------------------------------*/

/**
 * @brief Mapping from Caffeine ADC port IDs to global clock peripheral IDs.
 */
static const cfn_hal_port_peripheral_id_t PORT_MAP_CLOCK_PERIPHERAL_ID[CFN_HAL_ADC_PORT_MAX] = {
    [CFN_HAL_ADC_PORT_ADC1] = CFN_HAL_PORT_PERIPH_ADC1,
    [CFN_HAL_ADC_PORT_ADC2] = CFN_HAL_PORT_PERIPH_ADC2,
    [CFN_HAL_ADC_PORT_ADC3] = CFN_HAL_PORT_PERIPH_ADC3,
};

static const uint32_t PORT_MAP_RESOLUTION[CFN_HAL_ADC_RESOLUTION_BIT_MAX] = {
    [CFN_HAL_ADC_RESOLUTION_BIT_4]  = UINT32_MAX,         //
    [CFN_HAL_ADC_RESOLUTION_BIT_6]  = ADC_RESOLUTION_6B,  //
    [CFN_HAL_ADC_RESOLUTION_BIT_8]  = ADC_RESOLUTION_8B,  //
    [CFN_HAL_ADC_RESOLUTION_BIT_10] = ADC_RESOLUTION_10B, //
    [CFN_HAL_ADC_RESOLUTION_BIT_12] = ADC_RESOLUTION_12B, //
    [CFN_HAL_ADC_RESOLUTION_BIT_14] = UINT32_MAX,         //
    [CFN_HAL_ADC_RESOLUTION_BIT_16] = UINT32_MAX,         //
    [CFN_HAL_ADC_RESOLUTION_BIT_24] = UINT32_MAX,         //
};

static const uint32_t PORT_MAP_ALIGN[CFN_HAL_ADC_ALIGN_MAX] = {
    [CFN_HAL_ADC_ALIGN_RIGHT] = ADC_DATAALIGN_RIGHT,
    [CFN_HAL_ADC_ALIGN_LEFT]  = ADC_DATAALIGN_LEFT,
};

static const uint32_t PORT_MAP_SCAN[CFN_HAL_SCAN_MAX] = {
    [CFN_HAL_SCAN_ENABLED]  = ENABLE,
    [CFN_HAL_SCAN_DISABLED] = DISABLE,
};

static const uint32_t PORT_MAP_MODE[CFN_HAL_ADC_MODE_MAX] = {
    [CFN_HAL_ADC_MODE_SINGLE]     = ENABLE,
    [CFN_HAL_ADC_MODE_CONTINUOUS] = DISABLE,
};

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

static uint32_t get_port_id_from_handle(ADC_HandleTypeDef *handle)
{
    if ((handle < &port_hadcs[0]) || (handle >= &port_hadcs[CFN_HAL_ADC_PORT_MAX]))
    {
        return UINT32_MAX;
    }
    return (uint32_t) (handle - port_hadcs);
}

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t low_level_init(cfn_hal_adc_t *driver)
{
    if ((driver == NULL) || (driver->phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id >= CFN_HAL_ADC_PORT_MAX)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /* 1. Enable Clock */
    cfn_hal_port_clock_enable_gate(PORT_MAP_CLOCK_PERIPHERAL_ID[port_id]);

    /* 2. Initialize GPIO */
    if (driver->phy->gpio)
    {
        (void) cfn_hal_gpio_init(driver->phy->gpio->port);
    }

    return CFN_HAL_ERROR_OK;
}


static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_adc_t *driver = (cfn_hal_adc_t *) base;
    if ((driver == NULL) || (driver->phy == NULL) || (driver->config == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    cfn_hal_error_code_t err = cfn_hal_adc_config_validate(driver->config);
    if (err != CFN_HAL_ERROR_OK)
    {
        return err;
    }

    if (driver->api->base.config_validate != NULL)
    {
        err = driver->api->base.config_validate((cfn_hal_driver_t *) driver, driver->config);
        if (err != CFN_HAL_ERROR_OK)
        {
            return err;
        }
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id >= CFN_HAL_ADC_PORT_MAX)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    ADC_HandleTypeDef *hadc = &port_hadcs[port_id];

    err = low_level_init(driver);
    if (err != CFN_HAL_ERROR_OK)
    {
        return err;
    }


    uint32_t resolution = PORT_MAP_RESOLUTION[driver->config->resolution];
    uint32_t align      = PORT_MAP_ALIGN[driver->config->alignment];
    uint32_t scan       = PORT_MAP_SCAN[driver->config->scan];
    uint32_t mode       = PORT_MAP_MODE[driver->config->mode];
    uint32_t eoc        = ADC_EOC_SINGLE_CONV;
    if (CFN_HAL_BIT_CHECK(driver->config->eoc, CFN_HAL_ADC_EOC_SEQUENCE))
    {
        if (CFN_HAL_BIT_CHECK(driver->config->eoc, CFN_HAL_ADC_EOC_SINGLE))
        {
            eoc = ADC_EOC_SINGLE_SEQ_CONV;
        }
        else
        {
            eoc = ADC_EOC_SEQ_CONV;
        }
    }

    hadc->Instance                   = PORT_INSTANCES[port_id];
    hadc->Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc->Init.Resolution            = resolution;
    hadc->Init.ScanConvMode          = scan;
    hadc->Init.ContinuousConvMode    = mode;
    hadc->Init.DiscontinuousConvMode = DISABLE;
    hadc->Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc->Init.ExternalTrigConv      = ADC_SOFTWARE_START;
    hadc->Init.DataAlign             = align;
    hadc->Init.NbrOfConversion       = 1;
    hadc->Init.DMAContinuousRequests = DISABLE;
    hadc->Init.EOCSelection          = eoc;

    return cfn_hal_stm32_map_error(HAL_ADC_Init(hadc));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_adc_t *driver  = (cfn_hal_adc_t *) base;
    uint32_t       port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_ADC_DeInit(&port_hadcs[port_id]));
}

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(config);
    return port_base_init(base);
}

static cfn_hal_error_code_t port_base_event_enable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    cfn_hal_adc_t     *driver  = (cfn_hal_adc_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ADC_HandleTypeDef *hadc    = &port_hadcs[port_id];

    if (event_mask & CFN_HAL_ADC_EVENT_EOC)
    {
        __HAL_ADC_ENABLE_IT(hadc, ADC_IT_EOC);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    cfn_hal_adc_t     *driver  = (cfn_hal_adc_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ADC_HandleTypeDef *hadc    = &port_hadcs[port_id];

    if (event_mask & CFN_HAL_ADC_EVENT_EOC)
    {
        __HAL_ADC_DISABLE_IT(hadc, ADC_IT_EOC);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask)
{
    cfn_hal_adc_t     *driver  = (cfn_hal_adc_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ADC_HandleTypeDef *hadc    = &port_hadcs[port_id];
    uint32_t           mask    = 0;

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
    cfn_hal_adc_t     *driver  = (cfn_hal_adc_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ADC_HandleTypeDef *hadc    = &port_hadcs[port_id];

    if (error_mask & CFN_HAL_ADC_ERROR_OVERRUN)
    {
        __HAL_ADC_ENABLE_IT(hadc, ADC_IT_OVR);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_adc_t     *driver  = (cfn_hal_adc_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ADC_HandleTypeDef *hadc    = &port_hadcs[port_id];

    if (error_mask & CFN_HAL_ADC_ERROR_OVERRUN)
    {
        __HAL_ADC_DISABLE_IT(hadc, ADC_IT_OVR);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    cfn_hal_adc_t     *driver  = (cfn_hal_adc_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ADC_HandleTypeDef *hadc    = &port_hadcs[port_id];
    uint32_t           mask    = 0;

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
    uint32_t port_id = get_port_id_from_handle(hadc);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
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
    uint32_t port_id = get_port_id_from_handle(hadc);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
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

#ifndef CFN_HAL_PORT_DISABLE_IRQ_ADC

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

#endif /* CFN_HAL_PORT_DISABLE_IRQ_ADC */

/* ADC Specific Functions */

static cfn_hal_error_code_t port_adc_read(cfn_hal_adc_t *driver, uint32_t *data, uint32_t timeout)
{
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ADC_HandleTypeDef *hadc    = &port_hadcs[port_id];

    HAL_StatusTypeDef status   = HAL_ADC_Start(hadc);
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
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_ADC_Start_IT(&port_hadcs[port_id]));
}
static cfn_hal_error_code_t port_adc_stop(cfn_hal_adc_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_ADC_Stop_IT(&port_hadcs[port_id]));
}
static cfn_hal_error_code_t port_adc_read_dma(cfn_hal_adc_t *driver, uint32_t *data, size_t nbr_of_samples)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_ADC_Start_DMA(&port_hadcs[port_id], data, (uint32_t) nbr_of_samples));
}

/* API --------------------------------------------------------------*/
static const cfn_hal_adc_api_t ADC_API = {
    .base = {
        .init = port_base_init,
        .deinit = port_base_deinit,
        .power_state_set = NULL,
        .config_set = port_base_config_set,
        .callback_register = NULL,
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

#endif /* HAL_ADC_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_adc_construct(cfn_hal_adc_t *driver, const cfn_hal_adc_config_t *config, const cfn_hal_adc_phy_t *phy)
{
#ifdef HAL_ADC_MODULE_ENABLED
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_ADC_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api                  = &ADC_API;
    driver->base.type            = CFN_HAL_PERIPHERAL_TYPE_ADC;
    driver->base.status          = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config               = config;
    driver->phy                  = phy;

    port_hadcs[port_id].Instance = PORT_INSTANCES[port_id];
    port_drivers[port_id]        = driver;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    CFN_HAL_UNUSED(phy);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

cfn_hal_error_code_t cfn_hal_adc_destruct(cfn_hal_adc_t *driver)
{
#ifdef HAL_ADC_MODULE_ENABLED
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id < CFN_HAL_ADC_PORT_MAX)
    {
        port_drivers[port_id] = NULL;
    }

    driver->api         = NULL;
    driver->base.type   = CFN_HAL_PERIPHERAL_TYPE_ADC;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config      = NULL;
    driver->phy         = NULL;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}
