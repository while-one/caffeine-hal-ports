/**
 * @file cfn_hal_spi_port.c
 * @brief SPI HAL Template Implementation.
 */

#include "cfn_hal_spi.h"
#include "cfn_hal_spi_port.h"

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask) { CFN_HAL_UNUSED(base); if (event_mask) { *event_mask = 0; } return CFN_HAL_ERROR_OK; }

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask) { CFN_HAL_UNUSED(base); if (error_mask) { *error_mask = 0; } return CFN_HAL_ERROR_OK; }

static const cfn_hal_spi_api_t spi_api = {
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
    .xfr_irq = NULL,
    .xfr_irq_abort = NULL,
    .xfr_polling = NULL,
    .xfr_dma = NULL
};

cfn_hal_error_code_t cfn_hal_spi_construct(cfn_hal_spi_t *driver, const cfn_hal_spi_config_t *config, const cfn_hal_spi_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL)) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->api = &spi_api;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_SPI;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;
    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_spi_destruct(cfn_hal_spi_t *driver)
{
    if (driver == NULL) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_SPI;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;
    return CFN_HAL_ERROR_OK;
}
