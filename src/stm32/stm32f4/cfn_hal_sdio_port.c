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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file cfn_hal_sdio_port.c
 * @brief STM32F4 SDIO HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_sdio.h"
#include "cfn_hal_sdio_port.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_stm32_error.h"

#ifdef HAL_SD_MODULE_ENABLED

/* Private Data -----------------------------------------------------*/

static SD_HandleTypeDef port_hsd;

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t low_level_init(cfn_hal_sdio_t *driver)
{
    if ((driver == NULL) || (driver->phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /* 1. Enable Clock */
    __HAL_RCC_SDIO_CLK_ENABLE();

    /* 2. Initialize Pins */
    if (driver->phy->ck)
    {
        (void) cfn_hal_gpio_init(driver->phy->ck->port);
    }
    if (driver->phy->cmd)
    {
        (void) cfn_hal_gpio_init(driver->phy->cmd->port);
    }
    if (driver->phy->d0)
    {
        (void) cfn_hal_gpio_init(driver->phy->d0->port);
    }
    if (driver->phy->d1)
    {
        (void) cfn_hal_gpio_init(driver->phy->d1->port);
    }
    if (driver->phy->d2)
    {
        (void) cfn_hal_gpio_init(driver->phy->d2->port);
    }
    if (driver->phy->d3)
    {
        (void) cfn_hal_gpio_init(driver->phy->d3->port);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_sdio_t *driver = (cfn_hal_sdio_t *) base;

    cfn_hal_error_code_t err = cfn_hal_sdio_config_validate(driver->config);
    if (err != CFN_HAL_ERROR_OK)
    {
        return err;
    }

    if (driver->api->base.config_validate != NULL)
    {
        err = driver->api->base.config_validate((cfn_hal_driver_t *) driver, driver->config);
        if (err != CFN_HAL_ERROR_OK)
        {
            return err;
        }
    }

    err = low_level_init(driver);
    if (err != CFN_HAL_ERROR_OK)
    {
        return err;
    }

    port_hsd.Instance                 = SDIO;
    port_hsd.Init.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
    port_hsd.Init.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
    port_hsd.Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;
    port_hsd.Init.BusWide             = (driver->config->bus_wide == 4) ? SDIO_BUS_WIDE_4B : SDIO_BUS_WIDE_1B;
    port_hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    port_hsd.Init.ClockDiv            = 0; /* Target high speed */

    return cfn_hal_stm32_map_error(HAL_SD_Init(&port_hsd));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    CFN_HAL_UNUSED(base);
    return cfn_hal_stm32_map_error(HAL_SD_DeInit(&port_hsd));
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

static cfn_hal_error_code_t
port_sdio_send_command(cfn_hal_sdio_t *driver, const cfn_hal_sdio_cmd_t *cmd, uint32_t *response)
{
    CFN_HAL_UNUSED(driver);
    SDIO_CmdInitTypeDef s_cmd = { 0 };

    s_cmd.Argument            = cmd->arg;
    s_cmd.CmdIndex            = cmd->cmd_index;
    s_cmd.Response            = cmd->response_type;
    s_cmd.WaitForInterrupt =
        SDIO_WAIT_NO; /* Wait member exists in some F4 versions but might be named differently or used differently */
    s_cmd.CPSM = SDIO_CPSM_ENABLE;

    (void) SDIO_SendCommand(port_hsd.Instance, &s_cmd);

    if (response != NULL)
    {
        *response = SDIO_GetResponse(port_hsd.Instance, SDIO_RESP1);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_sdio_read_blocks(
    cfn_hal_sdio_t *driver, uint8_t *buffer, uint32_t block_addr, uint32_t nbr_of_blocks, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    return cfn_hal_stm32_map_error(HAL_SD_ReadBlocks(&port_hsd, buffer, block_addr, nbr_of_blocks, timeout));
}

static cfn_hal_error_code_t port_sdio_write_blocks(
    cfn_hal_sdio_t *driver, const uint8_t *data, uint32_t block_addr, uint32_t nbr_of_blocks, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    return cfn_hal_stm32_map_error(HAL_SD_WriteBlocks(&port_hsd, (uint8_t *) data, block_addr, nbr_of_blocks, timeout));
}

static cfn_hal_error_code_t port_sdio_get_card_info(cfn_hal_sdio_t *driver, cfn_hal_sdio_card_info_t *info)
{
    CFN_HAL_UNUSED(driver);
    HAL_SD_CardInfoTypeDef s_info;

    if (HAL_SD_GetCardInfo(&port_hsd, &s_info) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }

    info->card_type       = s_info.CardType;
    info->rel_card_addr   = s_info.RelCardAdd;
    info->block_count     = s_info.BlockNbr;
    info->block_size      = s_info.BlockSize;
    info->log_block_count = s_info.LogBlockNbr;
    info->log_block_size  = s_info.LogBlockSize;

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_sdio_wait_card_ready(cfn_hal_sdio_t *driver, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    uint32_t tickstart = HAL_GetTick();

    while (HAL_SD_GetCardState(&port_hsd) != HAL_SD_CARD_TRANSFER)
    {
        if ((HAL_GetTick() - tickstart) > timeout)
        {
            return CFN_HAL_ERROR_TIMING_TIMEOUT;
        }
    }

    return CFN_HAL_ERROR_OK;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_sdio_api_t SDIO_API = {
    .base = {
        .init = port_base_init,
        .deinit = port_base_deinit,
        .power_state_set = NULL,
        .config_set = port_base_config_set,
        .callback_register = NULL,
        .event_enable = NULL,
        .event_disable = NULL,
        .event_get = port_base_event_get,
        .error_enable = NULL,
        .error_disable = NULL,
        .error_get = port_base_error_get,
    },
    .send_command = port_sdio_send_command,
    .read_blocks = port_sdio_read_blocks,
    .write_blocks = port_sdio_write_blocks,
    .get_card_info = port_sdio_get_card_info,
    .wait_card_ready = port_sdio_wait_card_ready
};

#endif /* HAL_SD_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/
cfn_hal_error_code_t
cfn_hal_sdio_construct(cfn_hal_sdio_t *driver, const cfn_hal_sdio_config_t *config, const cfn_hal_sdio_phy_t *phy)
{
#ifdef HAL_SD_MODULE_ENABLED
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api         = &SDIO_API;
    driver->base.type   = CFN_HAL_PERIPHERAL_TYPE_SDIO;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config      = config;
    driver->phy         = phy;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    CFN_HAL_UNUSED(phy);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

cfn_hal_error_code_t cfn_hal_sdio_destruct(cfn_hal_sdio_t *driver)
{
#ifdef HAL_SD_MODULE_ENABLED
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api         = NULL;
    driver->base.type   = CFN_HAL_PERIPHERAL_TYPE_SDIO;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config      = NULL;
    driver->phy         = NULL;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}
