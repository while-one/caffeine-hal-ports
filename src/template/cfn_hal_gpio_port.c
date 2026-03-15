/**
 * @file cfn_hal_gpio_port.c
 * @brief GPIO HAL Template Implementation.
 */

#include "cfn_hal_gpio.h"
#include "cfn_hal_gpio_port.h"

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

static cfn_hal_error_code_t port_gpio_pin_config(cfn_hal_gpio_t *port, const cfn_hal_gpio_pin_config_t *pin_cfg)
{
    CFN_HAL_UNUSED(port);
    CFN_HAL_UNUSED(pin_cfg);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_gpio_pin_read(cfn_hal_gpio_t *port, cfn_hal_gpio_pin_t pin, cfn_hal_gpio_state_t *state)
{
    CFN_HAL_UNUSED(port);
    CFN_HAL_UNUSED(pin);
    CFN_HAL_UNUSED(state);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_gpio_pin_write(cfn_hal_gpio_t *port, cfn_hal_gpio_pin_t pin, cfn_hal_gpio_state_t state)
{
    CFN_HAL_UNUSED(port);
    CFN_HAL_UNUSED(pin);
    CFN_HAL_UNUSED(state);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_gpio_pin_toggle(cfn_hal_gpio_t *port, cfn_hal_gpio_pin_t pin)
{
    CFN_HAL_UNUSED(port);
    CFN_HAL_UNUSED(pin);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_gpio_port_read(cfn_hal_gpio_t *port, uint32_t *port_value)
{
    CFN_HAL_UNUSED(port);
    CFN_HAL_UNUSED(port_value);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_gpio_port_write(cfn_hal_gpio_t *port, uint32_t pin_mask, uint32_t port_value)
{
    CFN_HAL_UNUSED(port);
    CFN_HAL_UNUSED(pin_mask);
    CFN_HAL_UNUSED(port_value);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static const cfn_hal_gpio_api_t gpio_api = {
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
    .pin_config = port_gpio_pin_config,
    .pin_read = port_gpio_pin_read,
    .pin_write = port_gpio_pin_write,
    .pin_toggle = port_gpio_pin_toggle,
    .port_read = port_gpio_port_read,
    .port_write = port_gpio_port_write
};

cfn_hal_error_code_t cfn_hal_gpio_construct(cfn_hal_gpio_t *driver, void *config, const cfn_hal_gpio_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL)) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->api = &gpio_api;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_GPIO;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;
    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_gpio_destruct(cfn_hal_gpio_t *driver)
{
    if (driver == NULL) { return CFN_HAL_ERROR_BAD_PARAM; }
    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_GPIO;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;
    return CFN_HAL_ERROR_OK;
}
