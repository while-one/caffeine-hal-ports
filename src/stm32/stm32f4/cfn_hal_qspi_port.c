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
 * @file cfn_hal_qspi_port.c
 * @brief STM32F4 QSPI HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_qspi.h"
#include "cfn_hal_qspi_port.h"
#include "cfn_hal_stm32_error.h"

#ifdef HAL_QSPI_MODULE_ENABLED

/* Private Data -----------------------------------------------------*/

static QSPI_HandleTypeDef port_hqspi;

/* VMT Implementations ----------------------------------------------*/

static void low_level_init(cfn_hal_qspi_t *driver)
{
    /* 1. Enable Clock */
    __HAL_RCC_QSPI_CLK_ENABLE();

    /* 2. Initialize Pins */
    if (driver->phy->clk)
    {
        (void) cfn_hal_gpio_init(driver->phy->clk->port);
    }
    if (driver->phy->cs)
    {
        (void) cfn_hal_gpio_init(driver->phy->cs->port);
    }
    if (driver->phy->io0)
    {
        (void) cfn_hal_gpio_init(driver->phy->io0->port);
    }
    if (driver->phy->io1)
    {
        (void) cfn_hal_gpio_init(driver->phy->io1->port);
    }
    if (driver->phy->io2)
    {
        (void) cfn_hal_gpio_init(driver->phy->io2->port);
    }
    if (driver->phy->io3)
    {
        (void) cfn_hal_gpio_init(driver->phy->io3->port);
    }
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_qspi_t *driver = (cfn_hal_qspi_t *) base;

    low_level_init(driver);

    port_hqspi.Instance                = QUADSPI;
    port_hqspi.Init.ClockPrescaler     = 1;
    port_hqspi.Init.FifoThreshold      = 4;
    port_hqspi.Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    port_hqspi.Init.FlashSize          = 23; /* 16MB */
    port_hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
    port_hqspi.Init.ClockMode          = QSPI_CLOCK_MODE_0;
    port_hqspi.Init.FlashID            = QSPI_FLASH_ID_1;
    port_hqspi.Init.DualFlash          = QSPI_DUALFLASH_DISABLE;

    return cfn_hal_stm32_map_error(HAL_QSPI_Init(&port_hqspi));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    CFN_HAL_UNUSED(base);
    return cfn_hal_stm32_map_error(HAL_QSPI_DeInit(&port_hqspi));
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

static cfn_hal_error_code_t port_qspi_command(cfn_hal_qspi_t *driver, const cfn_hal_qspi_cmd_t *cmd, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    QSPI_CommandTypeDef s_cmd = { 0 };

    s_cmd.Instruction         = cmd->instruction;
    s_cmd.Address             = cmd->address;
    s_cmd.AlternateBytes      = cmd->alternate_bytes;
    s_cmd.AddressSize         = cmd->address_size;
    s_cmd.AlternateBytesSize  = cmd->alternate_bytes_size;
    s_cmd.DummyCycles         = cmd->dummy_cycles;
    s_cmd.InstructionMode     = cmd->instruction_mode;
    s_cmd.AddressMode         = cmd->address_mode;
    s_cmd.AlternateByteMode   = cmd->alternate_bytes_mode;
    s_cmd.DataMode            = cmd->data_mode;
    s_cmd.NbData              = cmd->nb_data;
    s_cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
    s_cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;

    return cfn_hal_stm32_map_error(HAL_QSPI_Command(&port_hqspi, &s_cmd, timeout));
}

static cfn_hal_error_code_t port_qspi_transmit(cfn_hal_qspi_t *driver, const uint8_t *data, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    return cfn_hal_stm32_map_error(HAL_QSPI_Transmit(&port_hqspi, (uint8_t *) data, timeout));
}

static cfn_hal_error_code_t port_qspi_receive(cfn_hal_qspi_t *driver, uint8_t *data, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    return cfn_hal_stm32_map_error(HAL_QSPI_Receive(&port_hqspi, data, timeout));
}

static cfn_hal_error_code_t port_qspi_memory_mapped_enable(cfn_hal_qspi_t *driver, const cfn_hal_qspi_cmd_t *cmd)
{
    CFN_HAL_UNUSED(driver);
    QSPI_CommandTypeDef      s_cmd        = { 0 };
    QSPI_MemoryMappedTypeDef s_mem_mapped = { 0 };

    s_cmd.Instruction                     = cmd->instruction;
    s_cmd.Address                         = cmd->address;
    s_cmd.AlternateBytes                  = cmd->alternate_bytes;
    s_cmd.AddressSize                     = cmd->address_size;
    s_cmd.AlternateBytesSize              = cmd->alternate_bytes_size;
    s_cmd.DummyCycles                     = cmd->dummy_cycles;
    s_cmd.InstructionMode                 = cmd->instruction_mode;
    s_cmd.AddressMode                     = cmd->address_mode;
    s_cmd.AlternateByteMode               = cmd->alternate_bytes_mode;
    s_cmd.DataMode                        = cmd->data_mode;
    s_cmd.NbData                          = cmd->nb_data;
    s_cmd.DdrMode                         = QSPI_DDR_MODE_DISABLE;
    s_cmd.SIOOMode                        = QSPI_SIOO_INST_EVERY_CMD;

    s_mem_mapped.TimeOutActivation        = QSPI_TIMEOUT_COUNTER_DISABLE;

    return cfn_hal_stm32_map_error(HAL_QSPI_MemoryMapped(&port_hqspi, &s_cmd, &s_mem_mapped));
}

/* API --------------------------------------------------------------*/
static const cfn_hal_qspi_api_t QSPI_API = {
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
    .command = port_qspi_command,
    .transmit = port_qspi_transmit,
    .receive = port_qspi_receive,
    .memory_mapped_enable = port_qspi_memory_mapped_enable
};

#endif /* HAL_QSPI_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/
cfn_hal_error_code_t
cfn_hal_qspi_construct(cfn_hal_qspi_t *driver, const cfn_hal_qspi_config_t *config, const cfn_hal_qspi_phy_t *phy)
{
#ifdef HAL_QSPI_MODULE_ENABLED
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api         = &QSPI_API;
    driver->base.type   = CFN_HAL_PERIPHERAL_TYPE_QSPI;
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

cfn_hal_error_code_t cfn_hal_qspi_destruct(cfn_hal_qspi_t *driver)
{
#ifdef HAL_QSPI_MODULE_ENABLED
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api         = NULL;
    driver->base.type   = CFN_HAL_PERIPHERAL_TYPE_QSPI;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config      = NULL;
    driver->phy         = NULL;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}
