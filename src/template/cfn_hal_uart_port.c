/**
 * @file cfn_hal_uart_port.c
 * @brief UART HAL Template Implementation.
 */

#include "cfn_hal_uart.h"
#include "cfn_hal_uart_port.h"

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask) { CFN_HAL_UNUSED(base); if (event_mask) { *event_mask = 0; } return CFN_HAL_ERROR_OK; }

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask) { CFN_HAL_UNUSED(base); if (error_mask) { *error_mask = 0; } return CFN_HAL_ERROR_OK; }

static cfn_hal_error_code_t port_uart_rx_n_irq(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes)
{
    if (driver) { driver->base.flags &= ~CFN_HAL_UART_FLAG_CONTINUOUS_RX; }
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(nbr_of_bytes);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_uart_rx_irq(cfn_hal_uart_t *driver)
{
    if (driver) { driver->base.flags |= CFN_HAL_UART_FLAG_CONTINUOUS_RX; }
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static const cfn_hal_uart_api_t uart_api = {
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
    .tx_irq = NULL,
    .tx_irq_abort = NULL,
    .rx_n_irq = port_uart_rx_n_irq,
    .rx_irq = port_uart_rx_irq,
    .rx_irq_abort = NULL,
    .rx_polling = NULL,
    .tx_dma = NULL,
    .rx_dma = NULL
};

cfn_hal_error_code_t cfn_hal_uart_construct(cfn_hal_uart_t *driver, const cfn_hal_uart_config_t *config, const cfn_hal_uart_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL)) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->api = &uart_api;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_UART;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;
    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_uart_destruct(cfn_hal_uart_t *driver)
{
    if (driver == NULL) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_UART;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;
    return CFN_HAL_ERROR_OK;
}
