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
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file cfn_hal_eth_port.c
 * @brief STM32F4 Ethernet HAL Port Implementation
 */

#include <string.h>
#include "cfn_hal_eth.h"

#ifdef HAL_ETH_MODULE_ENABLED

/* Private Variables ------------------------------------------------*/
static ETH_HandleTypeDef port_heths[CFN_HAL_ETH_PORT_MAX];
static cfn_hal_eth_t    *port_drivers[CFN_HAL_ETH_PORT_MAX];

static ETH_TypeDef *const PORT_INSTANCES[CFN_HAL_ETH_PORT_MAX] = {
    ETH,
};

static const uint32_t PORT_MAP_PERIPHERAL_ID[CFN_HAL_ETH_PORT_MAX] = {
    0, /* ETH is often unique or mapped differently in ID space */
};

/* Private Functions ------------------------------------------------*/

static uint32_t get_port_id_from_handle(const ETH_HandleTypeDef *heth)
{
    for (uint32_t i = 0; i < CFN_HAL_ETH_PORT_MAX; i++)
    {
        if (heth == &port_heths[i])
        {
            return i;
        }
    }
    return UINT32_MAX;
}

static cfn_hal_error_code_t low_level_init(cfn_hal_eth_t *driver)
{
    if (driver == NULL || driver->phy == NULL)
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

    /* 2. Initialize Pins (RMII usually) */
    if (driver->phy->ref_clk)
    {
        (void) cfn_hal_gpio_init(driver->phy->ref_clk->port);
    }
    if (driver->phy->mdio)
    {
        (void) cfn_hal_gpio_init(driver->phy->mdio->port);
    }
    if (driver->phy->mdc)
    {
        (void) cfn_hal_gpio_init(driver->phy->mdc->port);
    }
    if (driver->phy->crs_dv)
    {
        (void) cfn_hal_gpio_init(driver->phy->crs_dv->port);
    }
    if (driver->phy->rxd0)
    {
        (void) cfn_hal_gpio_init(driver->phy->rxd0->port);
    }
    if (driver->phy->rxd1)
    {
        (void) cfn_hal_gpio_init(driver->phy->rxd1->port);
    }
    if (driver->phy->tx_en)
    {
        (void) cfn_hal_gpio_init(driver->phy->tx_en->port);
    }
    if (driver->phy->txd0)
    {
        (void) cfn_hal_gpio_init(driver->phy->txd0->port);
    }
    if (driver->phy->txd1)
    {
        (void) cfn_hal_gpio_init(driver->phy->txd1->port);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_eth_t *driver = (cfn_hal_eth_t *) base;
    if ((driver == NULL) || (driver->phy == NULL) || (driver->config == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    ETH_HandleTypeDef *heth    = &port_heths[port_id];

    cfn_hal_error_code_t error = low_level_init(driver);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }

    heth->Instance            = PORT_INSTANCES[port_id];

    /* Simplified initialization for STM32F4 HAL */
    heth->Init.MACAddr        = (uint8_t *) driver->config->mac_addr;
    heth->Init.MediaInterface = HAL_ETH_RMII_MODE;
    heth->Init.RxMode         = ETH_RXPOLLING_MODE;
    heth->Init.ChecksumMode   = ETH_CHECKSUM_BY_HARDWARE;
    heth->Init.PhyAddress     = driver->config->phy_addr;

    return cfn_hal_stm32_map_error(HAL_ETH_Init(heth));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_eth_t *driver = (cfn_hal_eth_t *) base;
    if ((driver == NULL) || (driver->phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_ETH_DeInit(&port_heths[port_id]));
}

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(config);
    return port_base_init(base);
}

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask)
{
    CFN_HAL_UNUSED(base);
    if (event_mask != NULL)
    {
        *event_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    CFN_HAL_UNUSED(base);
    if (error_mask != NULL)
    {
        *error_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

/* ST HAL Callback Overrides ----------------------------------------*/

void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
    uint32_t port_id = get_port_id_from_handle(heth);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_eth_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_ETH_EVENT_RX_COMPLETE, 0, driver->cb_user_arg);
        }
    }
}

void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth)
{
    uint32_t port_id = get_port_id_from_handle(heth);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_eth_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_ETH_EVENT_TX_COMPLETE, 0, driver->cb_user_arg);
        }
    }
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

static cfn_hal_error_code_t
port_eth_transmit_frame(cfn_hal_eth_t *driver, const uint8_t *frame, size_t length, uint32_t timeout)
{
    uint32_t                  port_id   = (uint32_t) (uintptr_t) driver->phy->instance;
    ETH_TxPacketConfigTypeDef tx_config = { 0 };
    tx_config.Attributes                = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
    tx_config.Length                    = (uint32_t) length;
    tx_config.pData                     = (uint8_t *) frame;
    return cfn_hal_stm32_map_error(HAL_ETH_Transmit(&port_heths[port_id], &tx_config, timeout));
}

static cfn_hal_error_code_t port_eth_receive_frame(
    cfn_hal_eth_t *driver, uint8_t *buffer, size_t max_length, size_t *received_length, uint32_t timeout)
{
    CFN_HAL_UNUSED(timeout);
    uint32_t           port_id  = (uint32_t) (uintptr_t) driver->phy->instance;
    ETH_HandleTypeDef *heth     = &port_heths[port_id];
    void              *p_buffer = NULL;

    if (HAL_ETH_ReadData(heth, &p_buffer) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }

    uint32_t frame_length = heth->RxDescList.RxDataLength;
    if (frame_length > max_length)
    {
        frame_length = (uint32_t) max_length;
    }

    memcpy(buffer, p_buffer, frame_length);
    if (received_length != NULL)
    {
        *received_length = frame_length;
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_eth_read_phy_reg(cfn_hal_eth_t *driver, uint16_t phy_addr, uint16_t reg_addr, uint16_t *value, uint32_t timeout)
{
    CFN_HAL_UNUSED(timeout);
    uint32_t port_id  = (uint32_t) (uintptr_t) driver->phy->instance;
    uint32_t temp_val = 0;
    if (HAL_ETH_ReadPHYRegister(&port_heths[port_id], phy_addr, reg_addr, &temp_val) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }
    *value = (uint16_t) temp_val;
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_eth_write_phy_reg(cfn_hal_eth_t *driver, uint16_t phy_addr, uint16_t reg_addr, uint16_t value, uint32_t timeout)
{
    CFN_HAL_UNUSED(timeout);
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (HAL_ETH_WritePHYRegister(&port_heths[port_id], (uint32_t) phy_addr, (uint32_t) reg_addr, (uint32_t) value) !=
        HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_eth_get_link_status(cfn_hal_eth_t *driver, cfn_hal_eth_link_status_t *status, uint32_t timeout)
{
    uint16_t bsr      = 0;
    uint16_t phy_addr = driver->config->phy_addr;

    if (port_eth_read_phy_reg(driver, phy_addr, CFN_HAL_ETH_PHY_REG_BSR, &bsr, timeout) != CFN_HAL_ERROR_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }

    status->is_up = (bsr & CFN_HAL_ETH_PHY_BSR_LINKSTAT) != 0;

    if (!status->is_up)
    {
        status->speed  = CFN_HAL_ETH_LINK_SPEED_10M;
        status->duplex = CFN_HAL_ETH_LINK_DUPLEX_HALF;
        return CFN_HAL_ERROR_OK;
    }

    uint16_t bmcr = 0;
    if (port_eth_read_phy_reg(driver, phy_addr, CFN_HAL_ETH_PHY_REG_BMCR, &bmcr, timeout) != CFN_HAL_ERROR_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }

    if (bmcr & CFN_HAL_ETH_PHY_BMCR_AUTONEG) /* Auto-Negotiation Enable */
    {
        if (bsr & CFN_HAL_ETH_PHY_BSR_AUTONEGCMP) /* Auto-Negotiation Complete */
        {
            uint16_t anar   = 0;
            uint16_t anlpar = 0;
            port_eth_read_phy_reg(driver, phy_addr, CFN_HAL_ETH_PHY_REG_ANAR, &anar, timeout);
            port_eth_read_phy_reg(driver, phy_addr, CFN_HAL_ETH_PHY_REG_ANLPAR, &anlpar, timeout);

            uint16_t common_caps = anar & anlpar;
            if (common_caps & CFN_HAL_ETH_PHY_ANAR_100FD) /* 100Base-TX Full Duplex */
            {
                status->speed  = CFN_HAL_ETH_LINK_SPEED_100M;
                status->duplex = CFN_HAL_ETH_LINK_DUPLEX_FULL;
            }
            else if (common_caps & CFN_HAL_ETH_PHY_ANAR_100HD) /* 100Base-TX Half Duplex */
            {
                status->speed  = CFN_HAL_ETH_LINK_SPEED_100M;
                status->duplex = CFN_HAL_ETH_LINK_DUPLEX_HALF;
            }
            else if (common_caps & CFN_HAL_ETH_PHY_ANAR_10FD) /* 10Base-T Full Duplex */
            {
                status->speed  = CFN_HAL_ETH_LINK_SPEED_10M;
                status->duplex = CFN_HAL_ETH_LINK_DUPLEX_FULL;
            }
            else
            {
                status->speed  = CFN_HAL_ETH_LINK_SPEED_10M;
                status->duplex = CFN_HAL_ETH_LINK_DUPLEX_HALF;
            }
        }
        else
        {
            /* Fallback if auto-neg not complete but link is up */
            status->speed  = CFN_HAL_ETH_LINK_SPEED_100M;
            status->duplex = CFN_HAL_ETH_LINK_DUPLEX_FULL;
        }
    }
    else
    {
        status->speed =
            (bmcr & CFN_HAL_ETH_PHY_BMCR_SPEED_100) ? CFN_HAL_ETH_LINK_SPEED_100M : CFN_HAL_ETH_LINK_SPEED_10M;
        status->duplex =
            (bmcr & CFN_HAL_ETH_PHY_BMCR_DUPLEX) ? CFN_HAL_ETH_LINK_DUPLEX_FULL : CFN_HAL_ETH_LINK_DUPLEX_HALF;
    }

    return CFN_HAL_ERROR_OK;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_eth_api_t ETH_API = {
    .base =
        {
            .init = port_base_init,
            .deinit = port_base_deinit,
            .power_state_set = NULL,
            .config_set = port_base_config_set,
            .config_validate = NULL,
            .callback_register = NULL,
            .event_enable = NULL,
            .event_disable = NULL,
            .event_get = port_base_event_get,
            .error_enable = NULL,
            .error_disable = NULL,
            .error_get = port_base_error_get,
        },
    .start           = port_eth_start,
    .stop            = port_eth_stop,
    .transmit_frame  = port_eth_transmit_frame,
    .receive_frame   = port_eth_receive_frame,
    .read_phy_reg    = port_eth_read_phy_reg,
    .write_phy_reg   = port_eth_write_phy_reg,
    .get_link_status = port_eth_get_link_status};

#endif /* HAL_ETH_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t cfn_hal_eth_construct(cfn_hal_eth_t              *driver,
                                           const cfn_hal_eth_config_t *config,
                                           const cfn_hal_eth_phy_t    *phy,
                                           struct cfn_hal_clock_s     *clock,
                                           cfn_hal_eth_callback_t      callback,
                                           void                       *user_arg)
{
#ifdef HAL_ETH_MODULE_ENABLED
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_ETH_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t peripheral_id = PORT_MAP_PERIPHERAL_ID[port_id];

    cfn_hal_eth_populate(driver, peripheral_id, clock, &ETH_API, phy, config, callback, user_arg);

    port_heths[port_id].Instance = PORT_INSTANCES[port_id];
    port_drivers[port_id]        = driver;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    CFN_HAL_UNUSED(phy);
    CFN_HAL_UNUSED(clock);
    CFN_HAL_UNUSED(callback);
    CFN_HAL_UNUSED(user_arg);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

cfn_hal_error_code_t cfn_hal_eth_destruct(cfn_hal_eth_t *driver)
{
#ifdef HAL_ETH_MODULE_ENABLED
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (driver->phy != NULL)
    {
        uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
        if (port_id < CFN_HAL_ETH_PORT_MAX)
        {
            port_drivers[port_id] = NULL;
        }
    }

    driver->config = NULL;
    driver->phy    = NULL;
    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}
