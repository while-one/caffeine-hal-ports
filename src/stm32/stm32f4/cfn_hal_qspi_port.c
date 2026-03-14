/**
 * @file cfn_hal_qspi_port.c
 * @brief QSPI HAL Template Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_qspi.h"
#include "cfn_hal_qspi_port.h"

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

static cfn_hal_error_code_t port_qspi_command(cfn_hal_qspi_t *driver, const cfn_hal_qspi_cmd_t *cmd, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(cmd);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_qspi_transmit(cfn_hal_qspi_t *driver, const uint8_t *data, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_qspi_receive(cfn_hal_qspi_t *driver, uint8_t *data, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_qspi_memory_mapped_enable(cfn_hal_qspi_t *driver, const cfn_hal_qspi_cmd_t *cmd)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(cmd);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_qspi_api_t qspi_api = {
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
    .command = port_qspi_command,
    .transmit = port_qspi_transmit,
    .receive = port_qspi_receive,
    .memory_mapped_enable = port_qspi_memory_mapped_enable
};

/* Instantiation ----------------------------------------------------*/
cfn_hal_error_code_t cfn_hal_qspi_construct(cfn_hal_qspi_t *driver, const cfn_hal_qspi_config_t *config, const cfn_hal_qspi_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &qspi_api;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_QSPI;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_qspi_destruct(cfn_hal_qspi_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_QSPI;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
}
