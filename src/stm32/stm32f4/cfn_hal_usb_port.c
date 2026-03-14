/**
 * @file cfn_hal_usb_port.c
 * @brief USB HAL Template Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_usb.h"
#include "cfn_hal_usb_port.h"

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

static cfn_hal_error_code_t port_usb_start(cfn_hal_usb_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_usb_stop(cfn_hal_usb_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_usb_set_address(cfn_hal_usb_t *driver, uint8_t address)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(address);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_usb_ep_close(cfn_hal_usb_t *driver, uint8_t ep_addr)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(ep_addr);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_usb_ep_transmit(cfn_hal_usb_t *driver, uint8_t ep_addr, const uint8_t *data, size_t length)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(ep_addr);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(length);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_usb_ep_receive(cfn_hal_usb_t *driver, uint8_t ep_addr, uint8_t *buffer, size_t length)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(ep_addr);
    CFN_HAL_UNUSED(buffer);
    CFN_HAL_UNUSED(length);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_usb_ep_stall(cfn_hal_usb_t *driver, uint8_t ep_addr, bool stall)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(ep_addr);
    CFN_HAL_UNUSED(stall);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_usb_api_t usb_api = {
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
    .start = port_usb_start,
    .stop = port_usb_stop,
    .set_address = port_usb_set_address,
    .ep_close = port_usb_ep_close,
    .ep_transmit = port_usb_ep_transmit,
    .ep_receive = port_usb_ep_receive,
    .ep_stall = port_usb_ep_stall
};

/* Instantiation ----------------------------------------------------*/
cfn_hal_error_code_t cfn_hal_usb_construct(cfn_hal_usb_t *driver, const cfn_hal_usb_config_t *config, const cfn_hal_usb_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &usb_api;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_USB;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_usb_destruct(cfn_hal_usb_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_USB;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
}
