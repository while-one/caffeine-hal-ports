/**
 * @file cfn_hal_nvm_port.c
 * @brief NVM HAL Template Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_nvm.h"
#include "cfn_hal_nvm_port.h"

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    CFN_HAL_UNUSED(base);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    CFN_HAL_UNUSED(base);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_base_power_state_set(cfn_hal_driver_t *base, cfn_hal_power_state_t state)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(state);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(config);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_base_callback_register(cfn_hal_driver_t *base, cfn_hal_callback_t callback, void *user_arg)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(callback);
    CFN_HAL_UNUSED(user_arg);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_base_event_enable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(event_mask);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(event_mask);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(event_mask);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(error_mask);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(error_mask);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(error_mask);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_nvm_read(cfn_hal_nvm_t *driver, uint32_t addr, uint8_t *buffer, size_t size)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(addr);
    CFN_HAL_UNUSED(buffer);
    CFN_HAL_UNUSED(size);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_nvm_write(cfn_hal_nvm_t *driver, uint32_t addr, const uint8_t *data, size_t size)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(addr);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(size);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_nvm_erase_sector(cfn_hal_nvm_t *driver, uint32_t sector_addr)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(sector_addr);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_nvm_erase_chip(cfn_hal_nvm_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_nvm_get_info(cfn_hal_nvm_t *driver, cfn_hal_nvm_info_t *info)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(info);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_nvm_api_t nvm_api = {
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
    .read = port_nvm_read,
    .write = port_nvm_write,
    .erase_sector = port_nvm_erase_sector,
    .erase_chip = port_nvm_erase_chip,
    .get_info = port_nvm_get_info
};

/* Instantiation ----------------------------------------------------*/
cfn_hal_error_code_t cfn_hal_nvm_construct(cfn_hal_nvm_t *driver, const cfn_hal_nvm_config_t *config, const cfn_hal_nvm_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &nvm_api;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_NVM;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_nvm_destruct(cfn_hal_nvm_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_NVM;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
}
