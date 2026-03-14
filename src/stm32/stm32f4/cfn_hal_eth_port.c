/**
 * @file cfn_hal_eth_port.c
 * @brief STM32F4 ETH HAL Port Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_eth.h"
#include "cfn_hal_eth_port.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_stm32_error.h"

/* Private Data -----------------------------------------------------*/

static ETH_TypeDef *const PORT_INSTANCES[CFN_HAL_ETH_PORT_MAX] = {
#if defined(ETH)
    [CFN_HAL_ETH_PORT_ETH1] = ETH,
#endif
};

static ETH_HandleTypeDef port_heths[CFN_HAL_ETH_PORT_MAX];

/* VMT Implementations ----------------------------------------------*/

static void low_level_init(cfn_hal_eth_t *driver)
{
    /* 1. Enable Clock */
    cfn_hal_port_clock_enable_gate(CFN_HAL_PORT_PERIPH_ETH);

    /* 2. Initialize Pins */
    CFN_HAL_UNUSED(driver);
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_eth_t     *driver = (cfn_hal_eth_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ETH_HandleTypeDef *heth = &port_heths[port_id];

    low_level_init(driver);

    heth->Instance = PORT_INSTANCES[port_id];
    heth->Init.MACAddr = (uint8_t *) driver->config->mac_addr;
    heth->Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;

    return cfn_hal_stm32_map_error(HAL_ETH_Init(heth));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_eth_t *driver = (cfn_hal_eth_t *) base;
    uint32_t       port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_ETH_DeInit(&port_heths[port_id]));
}

static cfn_hal_error_code_t port_base_power_state_set(cfn_hal_driver_t *base, cfn_hal_power_state_t state)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(state);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(config);
    return port_base_init(base);
}
static cfn_hal_error_code_t
port_base_callback_register(cfn_hal_driver_t *base, cfn_hal_callback_t callback, void *user_arg)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(callback);
    CFN_HAL_UNUSED(user_arg);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_event_enable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(event_mask);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(event_mask);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask)
{
    CFN_HAL_UNUSED(base);
    if (event_mask)
    {
        *event_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(error_mask);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(error_mask);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    CFN_HAL_UNUSED(base);
    if (error_mask)
    {
        *error_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

/* ETH Specific Functions */

static cfn_hal_error_code_t port_eth_start(cfn_hal_eth_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_ETH_Start(&port_heths[port_id]));
}

static cfn_hal_error_code_t port_eth_stop(cfn_hal_eth_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_ETH_Stop(&port_heths[port_id]));
}

static cfn_hal_error_code_t port_eth_transmit_frame(cfn_hal_eth_t *driver, const uint8_t *frame, size_t length)
{
    uint32_t                  port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ETH_TxPacketConfigTypeDef tx_config = { 0 };
    tx_config.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
    tx_config.Length = (uint32_t) length;
    tx_config.pData = (uint8_t *) frame;
    return cfn_hal_stm32_map_error(HAL_ETH_Transmit(&port_heths[port_id], &tx_config, 100));
}

static cfn_hal_error_code_t
port_eth_receive_frame(cfn_hal_eth_t *driver, uint8_t *buffer, size_t max_length, size_t *received_length)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(buffer);
    CFN_HAL_UNUSED(max_length);
    CFN_HAL_UNUSED(received_length);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t
port_eth_read_phy_reg(cfn_hal_eth_t *driver, uint16_t phy_addr, uint16_t reg_addr, uint16_t *value)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(phy_addr);
    CFN_HAL_UNUSED(reg_addr);
    CFN_HAL_UNUSED(value);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t
port_eth_write_phy_reg(cfn_hal_eth_t *driver, uint16_t phy_addr, uint16_t reg_addr, uint16_t value)
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

/* API --------------------------------------------------------------*/
static const cfn_hal_eth_api_t ETH_API = {
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
    .receive_frame = port_eth_receive_frame,
    .read_phy_reg = port_eth_read_phy_reg,
    .write_phy_reg = port_eth_write_phy_reg,
    .get_link_status = port_eth_get_link_status
};

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_eth_construct(cfn_hal_eth_t *driver, const cfn_hal_eth_config_t *config, const cfn_hal_eth_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_ETH_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &ETH_API;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_ETH;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;

    port_heths[port_id].Instance = PORT_INSTANCES[port_id];

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_eth_destruct(cfn_hal_eth_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_ETH;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
}
