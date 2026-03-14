/**
 * @file cfn_hal_eth_port.c
 * @brief ETH HAL Template Implementation.
 */

#include "cfn_hal_eth.h"
#include "cfn_hal_eth_port.h"

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base) { CFN_HAL_UNUSED(base); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base) { CFN_HAL_UNUSED(base); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_power_state_set(cfn_hal_driver_t *base, cfn_hal_power_state_t state) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(state); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(config); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_callback_register(cfn_hal_driver_t *base, cfn_hal_callback_t callback, void *user_arg) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(callback); CFN_HAL_UNUSED(user_arg); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_event_enable(cfn_hal_driver_t *base, uint32_t event_mask) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(event_mask); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(event_mask); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask) { CFN_HAL_UNUSED(base); if (event_mask) { *event_mask = 0; } return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base, uint32_t error_mask) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(error_mask); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(error_mask); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask) { CFN_HAL_UNUSED(base); if (error_mask) { *error_mask = 0; } return CFN_HAL_ERROR_OK; }

static cfn_hal_error_code_t port_eth_start(cfn_hal_eth_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_eth_stop(cfn_hal_eth_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_eth_transmit_frame(cfn_hal_eth_t *driver, const uint8_t *frame, size_t length)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(frame);
    CFN_HAL_UNUSED(length);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_eth_read_phy_reg(cfn_hal_eth_t *driver, uint16_t phy_addr, uint16_t reg_addr, uint16_t *value)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(phy_addr);
    CFN_HAL_UNUSED(reg_addr);
    CFN_HAL_UNUSED(value);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_eth_write_phy_reg(cfn_hal_eth_t *driver, uint16_t phy_addr, uint16_t reg_addr, uint16_t value)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(phy_addr);
    CFN_HAL_UNUSED(reg_addr);
    CFN_HAL_UNUSED(value);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_eth_get_link_status(cfn_hal_eth_t *driver, cfn_hal_eth_link_status_t *status)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(status);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static const cfn_hal_eth_api_t eth_api = {
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
    .start = port_eth_start,
    .stop = port_eth_stop,
    .transmit_frame = port_eth_transmit_frame,
    .read_phy_reg = port_eth_read_phy_reg,
    .write_phy_reg = port_eth_write_phy_reg,
    .get_link_status = port_eth_get_link_status
};

cfn_hal_error_code_t cfn_hal_eth_construct(cfn_hal_eth_t *driver, const cfn_hal_eth_config_t *config, const cfn_hal_eth_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL)) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->api = &eth_api;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_ETH;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;
    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_eth_destruct(cfn_hal_eth_t *driver)
{
    if (driver == NULL) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_ETH;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;
    return CFN_HAL_ERROR_OK;
}
