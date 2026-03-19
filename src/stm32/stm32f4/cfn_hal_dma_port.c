/**
 * @file cfn_hal_dma_port.c
 * @brief STM32F4 DMA HAL Port Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_dma.h"
#include "cfn_hal_dma_port.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_stm32_error.h"

/* Private Data -----------------------------------------------------*/

static DMA_Stream_TypeDef *const PORT_INSTANCES[CFN_HAL_DMA_PORT_MAX] = {
    [CFN_HAL_DMA_PORT_DMA1_STREAM0] = DMA1_Stream0, [CFN_HAL_DMA_PORT_DMA1_STREAM1] = DMA1_Stream1,
    [CFN_HAL_DMA_PORT_DMA1_STREAM2] = DMA1_Stream2, [CFN_HAL_DMA_PORT_DMA1_STREAM3] = DMA1_Stream3,
    [CFN_HAL_DMA_PORT_DMA1_STREAM4] = DMA1_Stream4, [CFN_HAL_DMA_PORT_DMA1_STREAM5] = DMA1_Stream5,
    [CFN_HAL_DMA_PORT_DMA1_STREAM6] = DMA1_Stream6, [CFN_HAL_DMA_PORT_DMA1_STREAM7] = DMA1_Stream7,
    [CFN_HAL_DMA_PORT_DMA2_STREAM0] = DMA2_Stream0, [CFN_HAL_DMA_PORT_DMA2_STREAM1] = DMA2_Stream1,
    [CFN_HAL_DMA_PORT_DMA2_STREAM2] = DMA2_Stream2, [CFN_HAL_DMA_PORT_DMA2_STREAM3] = DMA2_Stream3,
    [CFN_HAL_DMA_PORT_DMA2_STREAM4] = DMA2_Stream4, [CFN_HAL_DMA_PORT_DMA2_STREAM5] = DMA2_Stream5,
    [CFN_HAL_DMA_PORT_DMA2_STREAM6] = DMA2_Stream6, [CFN_HAL_DMA_PORT_DMA2_STREAM7] = DMA2_Stream7,
};

static DMA_HandleTypeDef port_hdmas[CFN_HAL_DMA_PORT_MAX];
static cfn_hal_dma_t    *port_drivers[CFN_HAL_DMA_PORT_MAX];

/* Internal Helpers -------------------------------------------------*/

static int32_t get_port_id_from_handle(const DMA_HandleTypeDef *hdma)
{
    for (uint32_t i = 0; i < CFN_HAL_DMA_PORT_MAX; i++)
    {
        if (&port_hdmas[i] == hdma)
        {
            return (int32_t) i;
        }
    }
    return -1;
}

/* VMT Implementations ----------------------------------------------*/

static void low_level_init(cfn_hal_dma_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id < CFN_HAL_DMA_PORT_DMA2_STREAM0)
    {
        __HAL_RCC_DMA1_CLK_ENABLE();
    }
    else
    {
        __HAL_RCC_DMA2_CLK_ENABLE();
    }
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_dma_t     *driver = (cfn_hal_dma_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    DMA_HandleTypeDef *hdma = &port_hdmas[port_id];

    low_level_init(driver);

    hdma->Instance = PORT_INSTANCES[port_id];
    hdma->Init.Channel = (uint32_t) (uintptr_t) driver->phy->user_arg; /* Request Selection */
    hdma->Init.Direction = driver->config->direction;
    hdma->Init.PeriphInc = DMA_PINC_DISABLE;
    hdma->Init.MemInc = DMA_MINC_ENABLE;
    hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma->Init.Mode = DMA_NORMAL;
    hdma->Init.Priority = driver->config->priority;
    hdma->Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    return cfn_hal_stm32_map_error(HAL_DMA_Init(hdma));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_dma_t *driver = (cfn_hal_dma_t *) base;
    uint32_t       port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_DMA_DeInit(&port_hdmas[port_id]));
}

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(config);
    return port_base_init(base);
}

static cfn_hal_error_code_t port_base_event_enable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    cfn_hal_dma_t     *driver = (cfn_hal_dma_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    DMA_HandleTypeDef *hdma = &port_hdmas[port_id];

    if (event_mask & CFN_HAL_DMA_EVENT_TRANSFER_COMPLETE)
    {
        __HAL_DMA_ENABLE_IT(hdma, DMA_IT_TC);
    }
    if (event_mask & CFN_HAL_DMA_EVENT_TRANSFER_HALF)
    {
        __HAL_DMA_ENABLE_IT(hdma, DMA_IT_HT);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    cfn_hal_dma_t     *driver = (cfn_hal_dma_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    DMA_HandleTypeDef *hdma = &port_hdmas[port_id];

    if (event_mask & CFN_HAL_DMA_EVENT_TRANSFER_COMPLETE)
    {
        __HAL_DMA_DISABLE_IT(hdma, DMA_IT_TC);
    }
    if (event_mask & CFN_HAL_DMA_EVENT_TRANSFER_HALF)
    {
        __HAL_DMA_DISABLE_IT(hdma, DMA_IT_HT);
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
    cfn_hal_dma_t     *driver = (cfn_hal_dma_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    DMA_HandleTypeDef *hdma = &port_hdmas[port_id];

    if (error_mask != 0)
    {
        __HAL_DMA_ENABLE_IT(hdma, DMA_IT_TE | DMA_IT_FE | DMA_IT_DME);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_dma_t     *driver = (cfn_hal_dma_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    DMA_HandleTypeDef *hdma = &port_hdmas[port_id];

    if (error_mask != 0)
    {
        __HAL_DMA_DISABLE_IT(hdma, DMA_IT_TE | DMA_IT_FE | DMA_IT_DME);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    cfn_hal_dma_t     *driver = (cfn_hal_dma_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    DMA_HandleTypeDef *hdma = &port_hdmas[port_id];
    uint32_t           mask = 0;

    if (HAL_DMA_GetError(hdma) != HAL_DMA_ERROR_NONE)
    {
        mask |= CFN_HAL_DMA_ERROR_TRANSFER;
    }

    if (error_mask != NULL)
    {
        *error_mask = mask;
    }
    return CFN_HAL_ERROR_OK;
}

/* ST HAL Callback Overrides ----------------------------------------*/

void HAL_DMA_XferCpltCallback(const DMA_HandleTypeDef *hdma) // NOLINT(readability-identifier-naming)
{
    int32_t port_id = get_port_id_from_handle(hdma);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_dma_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_DMA_EVENT_TRANSFER_COMPLETE, CFN_HAL_DMA_ERROR_NONE, driver->cb_user_arg);
        }
    }
}

void HAL_DMA_XferHalfCpltCallback(const DMA_HandleTypeDef *hdma) // NOLINT(readability-identifier-naming)
{
    int32_t port_id = get_port_id_from_handle(hdma);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_dma_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_DMA_EVENT_TRANSFER_HALF, CFN_HAL_DMA_ERROR_NONE, driver->cb_user_arg);
        }
    }
}

void HAL_DMA_XferErrorCallback(const DMA_HandleTypeDef *hdma) // NOLINT(readability-identifier-naming)
{
    int32_t port_id = get_port_id_from_handle(hdma);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_dma_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            uint32_t error_mask = 0;
            (void) port_base_error_get(&driver->base, &error_mask);
            driver->cb(driver, CFN_HAL_DMA_EVENT_NONE, error_mask, driver->cb_user_arg);
        }
    }
}

/* Raw ISR Handlers -------------------------------------------------*/

#ifndef CFN_HAL_PORT_DISABLE_IRQ_DMA

void DMA1_Stream0_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA1_STREAM0]);
}
void DMA1_Stream1_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA1_STREAM1]);
}
void DMA1_Stream2_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA1_STREAM2]);
}
void DMA1_Stream3_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA1_STREAM3]);
}
void DMA1_Stream4_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA1_STREAM4]);
}
void DMA1_Stream5_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA1_STREAM5]);
}
void DMA1_Stream6_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA1_STREAM6]);
}
void DMA1_Stream7_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA1_STREAM7]);
}
void DMA2_Stream0_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA2_STREAM0]);
}
void DMA2_Stream1_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA2_STREAM1]);
}
void DMA2_Stream2_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA2_STREAM2]);
}
void DMA2_Stream3_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA2_STREAM3]);
}
void DMA2_Stream4_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA2_STREAM4]);
}
void DMA2_Stream5_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA2_STREAM5]);
}
void DMA2_Stream6_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA2_STREAM6]);
}
void DMA2_Stream7_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_DMA_IRQHandler(&port_hdmas[CFN_HAL_DMA_PORT_DMA2_STREAM7]);
}

#endif /* CFN_HAL_PORT_DISABLE_IRQ_DMA */

/* DMA Specific Functions */

static cfn_hal_error_code_t port_dma_start(cfn_hal_dma_t *driver, const cfn_hal_dma_transfer_t *transfer)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(
        HAL_DMA_Start_IT(&port_hdmas[port_id], transfer->src_addr, transfer->dst_addr, transfer->length));
}

static cfn_hal_error_code_t port_dma_stop(cfn_hal_dma_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_DMA_Abort_IT(&port_hdmas[port_id]));
}

static const cfn_hal_dma_api_t DMA_API = {
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
    .start = port_dma_start,
    .stop = port_dma_stop
};

cfn_hal_error_code_t
cfn_hal_dma_construct(cfn_hal_dma_t *driver, const cfn_hal_dma_config_t *config, const cfn_hal_dma_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_DMA_PORT_MAX)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &DMA_API;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_DMA;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;

    port_hdmas[port_id].Instance = PORT_INSTANCES[port_id];
    port_drivers[port_id] = driver;

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_dma_destruct(cfn_hal_dma_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id < CFN_HAL_DMA_PORT_MAX)
    {
        port_drivers[port_id] = NULL;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_DMA;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
}
