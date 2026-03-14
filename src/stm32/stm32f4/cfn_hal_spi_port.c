/**
 * @file cfn_hal_spi_port.c
 * @brief STM32F4 SPI HAL Port Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_spi.h"
#include "cfn_hal_spi_port.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_stm32_error.h"

/* Private Data -----------------------------------------------------*/

static SPI_TypeDef * const port_instances[CFN_HAL_SPI_PORT_MAX] = {
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

static SPI_HandleTypeDef port_hspis[CFN_HAL_SPI_PORT_MAX];

/* VMT Implementations ----------------------------------------------*/

static void low_level_init(cfn_hal_spi_t *driver)
{
    uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
    /* 1. Enable Clock */
    cfn_hal_port_clock_enable_gate((cfn_hal_port_peripheral_id_t)(CFN_HAL_PORT_PERIPH_SPI1 + port_id));

    /* 2. Initialize Pins */
    if (driver->phy->sck)  { (void)cfn_hal_base_init((cfn_hal_driver_t *)driver->phy->sck, CFN_HAL_PERIPHERAL_TYPE_GPIO); }
    if (driver->phy->miso) { (void)cfn_hal_base_init((cfn_hal_driver_t *)driver->phy->miso, CFN_HAL_PERIPHERAL_TYPE_GPIO); }
    if (driver->phy->mosi) { (void)cfn_hal_base_init((cfn_hal_driver_t *)driver->phy->mosi, CFN_HAL_PERIPHERAL_TYPE_GPIO); }
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_spi_t *driver = (cfn_hal_spi_t *)base;
    uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
    SPI_HandleTypeDef *hspi = &port_hspis[port_id];

    low_level_init(driver);

    hspi->Init.Mode = SPI_MODE_MASTER; 
    hspi->Init.Direction = SPI_DIRECTION_2LINES;
    
    hspi->Init.DataSize = (driver->config->data_size == 16) ? SPI_DATASIZE_16BIT : SPI_DATASIZE_8BIT;

    switch (driver->config->fmt)
    {
        case CFN_HAL_SPI_CONFIG_FMT_POL0_PHA0:
            hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
            hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
            break;
        case CFN_HAL_SPI_CONFIG_FMT_POL0_PHA1:
            hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
            hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
            break;
        case CFN_HAL_SPI_CONFIG_FMT_POL1_PHA0:
            hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
            hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
            break;
        case CFN_HAL_SPI_CONFIG_FMT_POL1_PHA1:
            hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
            hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
            break;
        default:
            return CFN_HAL_ERROR_BAD_PARAM;
    }

    hspi->Init.NSS = (driver->config->cs_mode == CFN_HAL_SPI_CONFIG_CS_HW_CONTROLLED) ? SPI_NSS_HARD_OUTPUT : SPI_NSS_SOFT;
    hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi->Init.TIMode = SPI_TIMODE_DISABLE;
    hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;

    return cfn_hal_stm32_map_error(HAL_SPI_Init(hspi));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_spi_t *driver = (cfn_hal_spi_t *)base;
    uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_SPI_DeInit(&port_hspis[port_id]));
}

/* ... standard base stubs ... */
static cfn_hal_error_code_t port_base_power_state_set(cfn_hal_driver_t *base, cfn_hal_power_state_t state) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(state); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(config); return port_base_init(base); }
static cfn_hal_error_code_t port_base_callback_register(cfn_hal_driver_t *base, cfn_hal_callback_t callback, void *user_arg) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(callback); CFN_HAL_UNUSED(user_arg); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_event_enable(cfn_hal_driver_t *base, uint32_t event_mask) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(event_mask); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(event_mask); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask) { CFN_HAL_UNUSED(base); if (event_mask) { *event_mask = 0; } return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base, uint32_t error_mask) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(error_mask); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(error_mask); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask) { CFN_HAL_UNUSED(base); if (error_mask) { *error_mask = 0; } return CFN_HAL_ERROR_OK; }

/* SPI Specific Functions */

static cfn_hal_error_code_t port_spi_xfr_polling(cfn_hal_spi_t *driver, const cfn_hal_spi_transaction_t *xfr, uint32_t timeout)
{
    uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
    SPI_HandleTypeDef *hspi = &port_hspis[port_id];
    HAL_StatusTypeDef status;

    if (driver->config->cs_mode == CFN_HAL_SPI_CONFIG_CS_HAL_CONTROLLED && xfr->cs)
    {
        (void)cfn_hal_gpio_pin_write(xfr->cs->port, xfr->cs->pin, CFN_HAL_GPIO_STATE_LOW);
    }

    if (xfr->tx_payload && xfr->rx_payload) {
        status = HAL_SPI_TransmitReceive(hspi, (uint8_t *)xfr->tx_payload, xfr->rx_payload, (uint16_t)xfr->nbr_of_bytes, timeout);
    } else if (xfr->tx_payload) {
        status = HAL_SPI_Transmit(hspi, (uint8_t *)xfr->tx_payload, (uint16_t)xfr->nbr_of_bytes, timeout);
    } else if (xfr->rx_payload) {
        status = HAL_SPI_Receive(hspi, xfr->rx_payload, (uint16_t)xfr->nbr_of_bytes, timeout);
    } else {
        status = HAL_OK;
    }

    if (driver->config->cs_mode == CFN_HAL_SPI_CONFIG_CS_HAL_CONTROLLED && xfr->cs)
    {
        (void)cfn_hal_gpio_pin_write(xfr->cs->port, xfr->cs->pin, CFN_HAL_GPIO_STATE_HIGH);
    }

    return cfn_hal_stm32_map_error(status);
}

static cfn_hal_error_code_t port_spi_xfr_irq(cfn_hal_spi_t *driver, const cfn_hal_spi_transaction_t *xfr) { CFN_HAL_UNUSED(driver); CFN_HAL_UNUSED(xfr); return CFN_HAL_ERROR_NOT_SUPPORTED; }
static cfn_hal_error_code_t port_spi_xfr_irq_abort(cfn_hal_spi_t *driver) { CFN_HAL_UNUSED(driver); return CFN_HAL_ERROR_NOT_SUPPORTED; }
static cfn_hal_error_code_t port_spi_xfr_dma(cfn_hal_spi_t *driver, const cfn_hal_spi_transaction_t *xfr) { CFN_HAL_UNUSED(driver); CFN_HAL_UNUSED(xfr); return CFN_HAL_ERROR_NOT_SUPPORTED; }

/* API --------------------------------------------------------------*/
static const cfn_hal_spi_api_t spi_api = {
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
    .xfr_irq = port_spi_xfr_irq,
    .xfr_irq_abort = port_spi_xfr_irq_abort,
    .xfr_polling = port_spi_xfr_polling,
    .xfr_dma = port_spi_xfr_dma
};

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t cfn_hal_spi_construct(cfn_hal_spi_t *driver, const cfn_hal_spi_config_t *config, const cfn_hal_spi_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t)(uintptr_t)phy->instance;
    if (port_id >= CFN_HAL_SPI_PORT_MAX || port_instances[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &spi_api;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_SPI;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;

    port_hspis[port_id].Instance = port_instances[port_id];

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_spi_destruct(cfn_hal_spi_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_SPI;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
}
