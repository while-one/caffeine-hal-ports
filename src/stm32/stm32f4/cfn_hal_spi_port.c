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
 * @file cfn_hal_spi_port.c
 * @brief STM32F4 SPI HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_spi_port.h"
#include "cfn_hal_clock.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_spi.h"
#include "cfn_hal_stm32_error.h"
#include "stm32f4xx_hal.h"

/* Private Prototypes ----------------------------------------------*/
#ifdef HAL_SPI_MODULE_ENABLED

/* Private Data -----------------------------------------------------*/
static SPI_TypeDef *const PORT_INSTANCES[CFN_HAL_SPI_PORT_MAX] = {
#if defined(SPI1)
    [CFN_HAL_SPI_PORT_1] = SPI1,
#endif
#if defined(SPI2)
    [CFN_HAL_SPI_PORT_2] = SPI2,
#endif
#if defined(SPI3)
    [CFN_HAL_SPI_PORT_3] = SPI3,
#endif
#if defined(SPI4)
    [CFN_HAL_SPI_PORT_4] = SPI4,
#endif
#if defined(SPI5)
    [CFN_HAL_SPI_PORT_5] = SPI5,
#endif
#if defined(SPI6)
    [CFN_HAL_SPI_PORT_6] = SPI6,
#endif
};

static const uint32_t PORT_MAP_PERIPHERAL_ID[CFN_HAL_SPI_PORT_MAX] = {
#if defined(SPI1)
    [CFN_HAL_SPI_PORT_1] = CFN_HAL_PORT_PERIPH_SPI1,
#endif
#if defined(SPI2)
    [CFN_HAL_SPI_PORT_2] = CFN_HAL_PORT_PERIPH_SPI2,
#endif
#if defined(SPI3)
    [CFN_HAL_SPI_PORT_3] = CFN_HAL_PORT_PERIPH_SPI3,
#endif
#if defined(SPI4)
    [CFN_HAL_SPI_PORT_4] = CFN_HAL_PORT_PERIPH_SPI4,
#endif
#if defined(SPI5)
    [CFN_HAL_SPI_PORT_5] = CFN_HAL_PORT_PERIPH_SPI5,
#endif
#if defined(SPI6)
    [CFN_HAL_SPI_PORT_6] = CFN_HAL_PORT_PERIPH_SPI6,
#endif
};

static SPI_HandleTypeDef port_hspis[CFN_HAL_SPI_PORT_MAX];
static cfn_hal_spi_t    *port_drivers[CFN_HAL_SPI_PORT_MAX];

/* Internal Helpers -------------------------------------------------*/

static uint32_t get_port_id_from_handle(SPI_HandleTypeDef *handle)
{
    if (!handle)
    {
        return UINT32_MAX;
    }
    if ((handle < &port_hspis[0]) || (handle >= &port_hspis[CFN_HAL_SPI_PORT_MAX]))
    {
        return UINT32_MAX;
    }
    return (uint32_t) (handle - port_hspis);
}

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t low_level_init(cfn_hal_spi_t *driver)
{
    if ((driver == NULL) || (driver->phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    struct cfn_hal_clock_s *clk = driver->base.clock_driver;
    if (clk == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /* 1. Enable Clock */
    cfn_hal_clock_enable_gate((cfn_hal_clock_t *) clk, driver->base.peripheral_id);

    /* 2. Initialize Pins */
    if (driver->phy->mosi)
    {
        (void) cfn_hal_gpio_init(driver->phy->mosi->port);
    }
    if (driver->phy->miso)
    {
        (void) cfn_hal_gpio_init(driver->phy->miso->port);
    }
    if (driver->phy->sck)
    {
        (void) cfn_hal_gpio_init(driver->phy->sck->port);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_spi_t *driver      = (cfn_hal_spi_t *) base;

    cfn_hal_error_code_t error = low_level_init(driver);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }

    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    SPI_HandleTypeDef *hspi    = &port_hspis[port_id];

    hspi->Instance             = PORT_INSTANCES[port_id];
    hspi->Init.Mode            = SPI_MODE_MASTER;
    hspi->Init.Direction       = SPI_DIRECTION_2LINES;
    hspi->Init.DataSize        = (driver->config->data_size == 16) ? SPI_DATASIZE_16BIT : SPI_DATASIZE_8BIT;

    switch (driver->config->fmt)
    {
        case CFN_HAL_SPI_CONFIG_FMT_POL0_PHA0:
            hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
            hspi->Init.CLKPhase    = SPI_PHASE_1EDGE;
            break;
        case CFN_HAL_SPI_CONFIG_FMT_POL0_PHA1:
            hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
            hspi->Init.CLKPhase    = SPI_PHASE_2EDGE;
            break;
        case CFN_HAL_SPI_CONFIG_FMT_POL1_PHA0:
            hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
            hspi->Init.CLKPhase    = SPI_PHASE_1EDGE;
            break;
        case CFN_HAL_SPI_CONFIG_FMT_POL1_PHA1:
            hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
            hspi->Init.CLKPhase    = SPI_PHASE_2EDGE;
            break;

        default:
            return CFN_HAL_ERROR_BAD_PARAM;
    }

    hspi->Init.NSS       = SPI_NSS_SOFT;

    uint32_t periph_freq = 0;
    (void) cfn_hal_clock_get_peripheral_freq(
        (cfn_hal_clock_t *) driver->base.clock_driver, driver->base.peripheral_id, &periph_freq);

    uint32_t target_bitrate = driver->config->bitrate;
    uint32_t prescaler      = SPI_BAUDRATEPRESCALER_256;

    if ((periph_freq > 0) && (target_bitrate > 0))
    {
        uint32_t div = periph_freq / target_bitrate;
        if (div <= 2)
        {
            prescaler = SPI_BAUDRATEPRESCALER_2;
        }
        else if (div <= 4)
        {
            prescaler = SPI_BAUDRATEPRESCALER_4;
        }
        else if (div <= 8)
        {
            prescaler = SPI_BAUDRATEPRESCALER_8;
        }
        else if (div <= 16)
        {
            prescaler = SPI_BAUDRATEPRESCALER_16;
        }
        else if (div <= 32)
        {
            prescaler = SPI_BAUDRATEPRESCALER_32;
        }
        else if (div <= 64)
        {
            prescaler = SPI_BAUDRATEPRESCALER_64;
        }
        else if (div <= 128)
        {
            prescaler = SPI_BAUDRATEPRESCALER_128;
        }
        else
        {
            prescaler = SPI_BAUDRATEPRESCALER_256;
        }
    }

    hspi->Init.BaudRatePrescaler = prescaler;
    hspi->Init.FirstBit          = SPI_FIRSTBIT_MSB;
    hspi->Init.TIMode            = SPI_TIMODE_DISABLE;
    hspi->Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    hspi->Init.CRCPolynomial     = 10;

    return cfn_hal_stm32_map_error(HAL_SPI_Init(hspi));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_spi_t *driver  = (cfn_hal_spi_t *) base;
    uint32_t       port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_SPI_DeInit(&port_hspis[port_id]));
}

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(config);
    return port_base_init(base);
}

static cfn_hal_error_code_t port_base_event_enable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    cfn_hal_spi_t     *driver  = (cfn_hal_spi_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    SPI_HandleTypeDef *hspi    = &port_hspis[port_id];

    if (event_mask & CFN_HAL_SPI_EVENT_TX_COMPLETE)
    {
        __HAL_SPI_ENABLE_IT(hspi, SPI_IT_TXE);
    }
    if (event_mask & CFN_HAL_SPI_EVENT_RX_READY)
    {
        __HAL_SPI_ENABLE_IT(hspi, SPI_IT_RXNE);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    cfn_hal_spi_t     *driver  = (cfn_hal_spi_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    SPI_HandleTypeDef *hspi    = &port_hspis[port_id];

    if (event_mask & CFN_HAL_SPI_EVENT_TX_COMPLETE)
    {
        __HAL_SPI_DISABLE_IT(hspi, SPI_IT_TXE);
    }
    if (event_mask & CFN_HAL_SPI_EVENT_RX_READY)
    {
        __HAL_SPI_DISABLE_IT(hspi, SPI_IT_RXNE);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask)
{
    cfn_hal_spi_t     *driver  = (cfn_hal_spi_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    SPI_HandleTypeDef *hspi    = &port_hspis[port_id];
    uint32_t           mask    = 0;

    if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE))
    {
        mask |= CFN_HAL_SPI_EVENT_TX_COMPLETE;
    }
    if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE))
    {
        mask |= CFN_HAL_SPI_EVENT_RX_READY;
    }

    if (event_mask != NULL)
    {
        *event_mask = mask;
    }
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_spi_t     *driver  = (cfn_hal_spi_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    SPI_HandleTypeDef *hspi    = &port_hspis[port_id];

    if (error_mask & (CFN_HAL_SPI_ERROR_CRC | CFN_HAL_SPI_ERROR_FRAMING | CFN_HAL_SPI_ERROR_OVERRUN))
    {
        __HAL_SPI_ENABLE_IT(hspi, SPI_IT_ERR);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_spi_t     *driver  = (cfn_hal_spi_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    SPI_HandleTypeDef *hspi    = &port_hspis[port_id];

    if (error_mask & (CFN_HAL_SPI_ERROR_CRC | CFN_HAL_SPI_ERROR_FRAMING | CFN_HAL_SPI_ERROR_OVERRUN))
    {
        __HAL_SPI_DISABLE_IT(hspi, SPI_IT_ERR);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    cfn_hal_spi_t     *driver  = (cfn_hal_spi_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    SPI_HandleTypeDef *hspi    = &port_hspis[port_id];
    uint32_t           mask    = 0;

    if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR))
    {
        mask |= CFN_HAL_SPI_ERROR_CRC;
    }
    if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_FRE))
    {
        mask |= CFN_HAL_SPI_ERROR_FRAMING;
    }
    if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_OVR))
    {
        mask |= CFN_HAL_SPI_ERROR_OVERRUN;
    }

    if (error_mask != NULL)
    {
        *error_mask = mask;
    }
    return CFN_HAL_ERROR_OK;
}

/* ST HAL Callback Overrides ----------------------------------------*/

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    uint32_t port_id = get_port_id_from_handle(hspi);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_spi_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_SPI_EVENT_TX_COMPLETE, CFN_HAL_SPI_ERROR_NONE, NULL, 0, driver->cb_user_arg);
        }
    }
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    uint32_t port_id = get_port_id_from_handle(hspi);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_spi_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_SPI_EVENT_RX_READY, CFN_HAL_SPI_ERROR_NONE, NULL, 0, driver->cb_user_arg);
        }
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    uint32_t port_id = get_port_id_from_handle(hspi);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_spi_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver,
                       CFN_HAL_SPI_EVENT_TX_COMPLETE | CFN_HAL_SPI_EVENT_RX_READY,
                       CFN_HAL_SPI_ERROR_NONE,
                       NULL,
                       0,
                       driver->cb_user_arg);
        }
    }
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    uint32_t port_id = get_port_id_from_handle(hspi);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_spi_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            uint32_t error_mask = 0;
            (void) port_base_error_get(&driver->base, &error_mask);
            driver->cb(driver, CFN_HAL_SPI_EVENT_NONE, error_mask, NULL, 0, driver->cb_user_arg);
        }
    }
}

/* Raw ISR Handlers -------------------------------------------------*/

#ifndef CFN_HAL_PORT_DISABLE_IRQ_SPI
void SPI1_IRQHandler(void); // NOLINT(readability-identifier-naming)
void SPI2_IRQHandler(void); // NOLINT(readability-identifier-naming)
void SPI3_IRQHandler(void); // NOLINT(readability-identifier-naming)

#if defined(SPI1)
void SPI1_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_SPI_IRQHandler(&port_hspis[CFN_HAL_SPI_PORT_1]);
}
#endif

#if defined(SPI2)
void SPI2_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_SPI_IRQHandler(&port_hspis[CFN_HAL_SPI_PORT_2]);
}
#endif

#if defined(SPI3)
void SPI3_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_SPI_IRQHandler(&port_hspis[CFN_HAL_SPI_PORT_3]);
}
#endif

#endif /* CFN_HAL_PORT_DISABLE_IRQ_SPI */

/* SPI Specific Functions */

static cfn_hal_error_code_t
port_spi_xfr_polling(cfn_hal_spi_t *driver, const cfn_hal_spi_transaction_t *xfr, uint32_t timeout)
{
    uint32_t          port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    HAL_StatusTypeDef status;

    if (xfr->tx_payload && xfr->rx_payload)
    {
        status = HAL_SPI_TransmitReceive(
            &port_hspis[port_id], (uint8_t *) xfr->tx_payload, xfr->rx_payload, (uint16_t) xfr->nbr_of_bytes, timeout);
    }
    else if (xfr->tx_payload)
    {
        status =
            HAL_SPI_Transmit(&port_hspis[port_id], (uint8_t *) xfr->tx_payload, (uint16_t) xfr->nbr_of_bytes, timeout);
    }
    else if (xfr->rx_payload)
    {
        status = HAL_SPI_Receive(&port_hspis[port_id], xfr->rx_payload, (uint16_t) xfr->nbr_of_bytes, timeout);
    }
    else
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    return cfn_hal_stm32_map_error(status);
}

static cfn_hal_error_code_t port_spi_xfr_irq(cfn_hal_spi_t *driver, const cfn_hal_spi_transaction_t *xfr)
{
    uint32_t          port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    HAL_StatusTypeDef status;

    if (xfr->tx_payload && xfr->rx_payload)
    {
        status = HAL_SPI_TransmitReceive_IT(
            &port_hspis[port_id], (uint8_t *) xfr->tx_payload, xfr->rx_payload, (uint16_t) xfr->nbr_of_bytes);
    }
    else if (xfr->tx_payload)
    {
        status = HAL_SPI_Transmit_IT(&port_hspis[port_id], (uint8_t *) xfr->tx_payload, (uint16_t) xfr->nbr_of_bytes);
    }
    else if (xfr->rx_payload)
    {
        status = HAL_SPI_Receive_IT(&port_hspis[port_id], xfr->rx_payload, (uint16_t) xfr->nbr_of_bytes);
    }
    else
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    return cfn_hal_stm32_map_error(status);
}

static cfn_hal_error_code_t port_spi_xfr_irq_abort(cfn_hal_spi_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_SPI_Abort_IT(&port_hspis[port_id]));
}

/* API --------------------------------------------------------------*/
static const cfn_hal_spi_api_t SPI_API = {
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
    .xfr_irq = port_spi_xfr_irq,
    .xfr_irq_abort = port_spi_xfr_irq_abort,
    .xfr_polling = port_spi_xfr_polling,
    .xfr_dma = NULL};

#endif /* HAL_SPI_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t cfn_hal_spi_construct(cfn_hal_spi_t              *driver,
                                           const cfn_hal_spi_config_t *config,
                                           const cfn_hal_spi_phy_t    *phy,
                                           struct cfn_hal_clock_s     *clock,
                                           cfn_hal_spi_callback_t      callback,
                                           void                       *user_arg)
{
#ifdef HAL_SPI_MODULE_ENABLED
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_SPI_PORT_MAX)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t peripheral_id = PORT_MAP_PERIPHERAL_ID[port_id];

    cfn_hal_spi_populate(driver, peripheral_id, clock, &SPI_API, phy, config, callback, user_arg);

    port_hspis[port_id].Instance = PORT_INSTANCES[port_id];
    port_drivers[port_id]        = driver;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(port_id);
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    CFN_HAL_UNUSED(phy);
    CFN_HAL_UNUSED(clock);
    CFN_HAL_UNUSED(callback);
    CFN_HAL_UNUSED(user_arg);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

cfn_hal_error_code_t cfn_hal_spi_destruct(cfn_hal_spi_t *driver)
{
#ifdef HAL_SPI_MODULE_ENABLED
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id < CFN_HAL_SPI_PORT_MAX)
    {
        port_drivers[port_id] = NULL;
    }

    driver->config = NULL;
    driver->phy    = NULL;
    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}
