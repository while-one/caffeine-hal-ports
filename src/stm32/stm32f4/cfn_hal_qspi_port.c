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
#include "cfn_hal_qspi_port.h"
#include "cfn_hal_clock.h"
#include "cfn_hal_qspi.h"
#include "cfn_hal_stm32_error.h"
#include "stm32f4xx_hal.h"

#ifdef HAL_QSPI_MODULE_ENABLED

/* Private Data -----------------------------------------------------*/

/**
 * @brief Mapping from Caffeine QSPI port IDs to global clock peripheral IDs.
 */
static const uint32_t PORT_MAP_PERIPHERAL_ID[CFN_HAL_QSPI_PORT_MAX] = {
    [CFN_HAL_QSPI_PORT_QSPI1] = CFN_HAL_PORT_PERIPH_QSPI,
};

static QSPI_TypeDef *const PORT_INSTANCES[CFN_HAL_QSPI_PORT_MAX] = {
#if defined(QUADSPI)
    [CFN_HAL_QSPI_PORT_QSPI1] = QUADSPI,
#endif
};

static QSPI_HandleTypeDef port_hqspis[CFN_HAL_QSPI_PORT_MAX];
static cfn_hal_qspi_t    *port_drivers[CFN_HAL_QSPI_PORT_MAX];

/* Internal Helpers -------------------------------------------------*/

static uint32_t get_port_id_from_handle(QSPI_HandleTypeDef *handle)
{
    if (!handle)
    {
        return UINT32_MAX;
    }
    if ((handle < &port_hqspis[0]) || (handle >= &port_hqspis[CFN_HAL_QSPI_PORT_MAX]))
    {
        return UINT32_MAX;
    }
    return (uint32_t) (handle - port_hqspis);
}

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t low_level_init(cfn_hal_qspi_t *driver)
{
    if (driver == NULL || driver->phy == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

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

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_qspi_t *driver     = (cfn_hal_qspi_t *) base;
    uint32_t        port_id    = (uint32_t) (uintptr_t) driver->phy->instance;

    cfn_hal_error_code_t error = low_level_init(driver);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }

    QSPI_HandleTypeDef *hqspi      = &port_hqspis[port_id];

    hqspi->Instance                = PORT_INSTANCES[port_id];
    hqspi->Init.ClockPrescaler     = driver->config->clock_prescaler;
    hqspi->Init.FifoThreshold      = 4;
    hqspi->Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    hqspi->Init.FlashSize          = driver->config->flash_size;
    hqspi->Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
    hqspi->Init.ClockMode          = QSPI_CLOCK_MODE_0;
    hqspi->Init.FlashID            = QSPI_FLASH_ID_1;
    hqspi->Init.DualFlash          = QSPI_DUALFLASH_DISABLE;

    return cfn_hal_stm32_map_error(HAL_QSPI_Init(hqspi));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_qspi_t *driver  = (cfn_hal_qspi_t *) base;
    uint32_t        port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_QSPI_DeInit(&port_hqspis[port_id]));
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
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    QSPI_CommandTypeDef s_cmd   = { 0 };

    s_cmd.Instruction           = cmd->instruction;
    s_cmd.Address               = cmd->address;
    s_cmd.AlternateBytes        = 0;
    s_cmd.AddressSize           = QSPI_ADDRESS_32BITS;
    s_cmd.AlternateBytesSize    = QSPI_ALTERNATE_BYTES_8BITS;
    s_cmd.DummyCycles           = cmd->dummy_cycles;
    s_cmd.InstructionMode       = (cmd->instruction_mode == CFN_HAL_QSPI_BUS_WIDTH_SINGLE) ? QSPI_INSTRUCTION_1_LINE
                                  : (cmd->instruction_mode == CFN_HAL_QSPI_BUS_WIDTH_DUAL) ? QSPI_INSTRUCTION_2_LINES
                                  : (cmd->instruction_mode == CFN_HAL_QSPI_BUS_WIDTH_QUAD) ? QSPI_INSTRUCTION_4_LINES
                                                                                           : QSPI_INSTRUCTION_NONE;
    s_cmd.AddressMode           = (cmd->address_mode == CFN_HAL_QSPI_BUS_WIDTH_SINGLE) ? QSPI_ADDRESS_1_LINE
                                  : (cmd->address_mode == CFN_HAL_QSPI_BUS_WIDTH_DUAL) ? QSPI_ADDRESS_2_LINES
                                  : (cmd->address_mode == CFN_HAL_QSPI_BUS_WIDTH_QUAD) ? QSPI_ADDRESS_4_LINES
                                                                                       : QSPI_ADDRESS_NONE;
    s_cmd.AlternateByteMode     = QSPI_ALTERNATE_BYTES_NONE;
    s_cmd.DataMode              = (cmd->data_mode == CFN_HAL_QSPI_BUS_WIDTH_SINGLE) ? QSPI_DATA_1_LINE
                                  : (cmd->data_mode == CFN_HAL_QSPI_BUS_WIDTH_DUAL) ? QSPI_DATA_2_LINES
                                  : (cmd->data_mode == CFN_HAL_QSPI_BUS_WIDTH_QUAD) ? QSPI_DATA_4_LINES
                                                                                    : QSPI_DATA_NONE;
    s_cmd.NbData                = cmd->nbr_of_data;
    s_cmd.DdrMode               = QSPI_DDR_MODE_DISABLE;
    s_cmd.SIOOMode              = QSPI_SIOO_INST_EVERY_CMD;

    return cfn_hal_stm32_map_error(HAL_QSPI_Command(&port_hqspis[port_id], &s_cmd, timeout));
}

static cfn_hal_error_code_t port_qspi_transmit(cfn_hal_qspi_t *driver, const uint8_t *data, uint32_t timeout)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_QSPI_Transmit(&port_hqspis[port_id], (uint8_t *) data, timeout));
}

static cfn_hal_error_code_t port_qspi_receive(cfn_hal_qspi_t *driver, uint8_t *data, uint32_t timeout)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_QSPI_Receive(&port_hqspis[port_id], data, timeout));
}

static cfn_hal_error_code_t port_qspi_memory_mapped_enable(cfn_hal_qspi_t *driver, const cfn_hal_qspi_cmd_t *cmd)
{
    uint32_t                 port_id      = (uint32_t) (uintptr_t) driver->phy->instance;
    QSPI_CommandTypeDef      s_cmd        = { 0 };
    QSPI_MemoryMappedTypeDef s_mem_mapped = { 0 };

    s_cmd.Instruction                     = cmd->instruction;
    s_cmd.Address                         = cmd->address;
    s_cmd.AlternateBytes                  = 0;
    s_cmd.AddressSize                     = QSPI_ADDRESS_32BITS;
    s_cmd.AlternateBytesSize              = QSPI_ALTERNATE_BYTES_8BITS;
    s_cmd.DummyCycles                     = cmd->dummy_cycles;
    s_cmd.InstructionMode          = (cmd->instruction_mode == CFN_HAL_QSPI_BUS_WIDTH_SINGLE) ? QSPI_INSTRUCTION_1_LINE
                                     : (cmd->instruction_mode == CFN_HAL_QSPI_BUS_WIDTH_DUAL) ? QSPI_INSTRUCTION_2_LINES
                                     : (cmd->instruction_mode == CFN_HAL_QSPI_BUS_WIDTH_QUAD) ? QSPI_INSTRUCTION_4_LINES
                                                                                              : QSPI_INSTRUCTION_NONE;
    s_cmd.AddressMode              = (cmd->address_mode == CFN_HAL_QSPI_BUS_WIDTH_SINGLE) ? QSPI_ADDRESS_1_LINE
                                     : (cmd->address_mode == CFN_HAL_QSPI_BUS_WIDTH_DUAL) ? QSPI_ADDRESS_2_LINES
                                     : (cmd->address_mode == CFN_HAL_QSPI_BUS_WIDTH_QUAD) ? QSPI_ADDRESS_4_LINES
                                                                                          : QSPI_ADDRESS_NONE;
    s_cmd.AlternateByteMode        = QSPI_ALTERNATE_BYTES_NONE;
    s_cmd.DataMode                 = (cmd->data_mode == CFN_HAL_QSPI_BUS_WIDTH_SINGLE) ? QSPI_DATA_1_LINE
                                     : (cmd->data_mode == CFN_HAL_QSPI_BUS_WIDTH_DUAL) ? QSPI_DATA_2_LINES
                                     : (cmd->data_mode == CFN_HAL_QSPI_BUS_WIDTH_QUAD) ? QSPI_DATA_4_LINES
                                                                                       : QSPI_DATA_NONE;
    s_cmd.NbData                   = cmd->nbr_of_data;
    s_cmd.DdrMode                  = QSPI_DDR_MODE_DISABLE;
    s_cmd.SIOOMode                 = QSPI_SIOO_INST_EVERY_CMD;

    s_mem_mapped.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;

    return cfn_hal_stm32_map_error(HAL_QSPI_MemoryMapped(&port_hqspis[port_id], &s_cmd, &s_mem_mapped));
}

static cfn_hal_error_code_t port_qspi_autopolling_enable(
    cfn_hal_qspi_t *driver, const cfn_hal_qspi_cmd_t *cmd, uint32_t match, uint32_t mask, uint32_t timeout)
{
    uint32_t                port_id  = (uint32_t) (uintptr_t) driver->phy->instance;
    QSPI_CommandTypeDef     s_cmd    = { 0 };
    QSPI_AutoPollingTypeDef s_config = { 0 };

    s_cmd.Instruction                = cmd->instruction;
    s_cmd.Address                    = cmd->address;
    s_cmd.AlternateBytes             = 0;
    s_cmd.AddressSize                = QSPI_ADDRESS_32BITS;
    s_cmd.AlternateBytesSize         = QSPI_ALTERNATE_BYTES_8BITS;
    s_cmd.DummyCycles                = cmd->dummy_cycles;
    s_cmd.InstructionMode   = (cmd->instruction_mode == CFN_HAL_QSPI_BUS_WIDTH_SINGLE) ? QSPI_INSTRUCTION_1_LINE
                              : (cmd->instruction_mode == CFN_HAL_QSPI_BUS_WIDTH_DUAL) ? QSPI_INSTRUCTION_2_LINES
                              : (cmd->instruction_mode == CFN_HAL_QSPI_BUS_WIDTH_QUAD) ? QSPI_INSTRUCTION_4_LINES
                                                                                       : QSPI_INSTRUCTION_NONE;
    s_cmd.AddressMode       = (cmd->address_mode == CFN_HAL_QSPI_BUS_WIDTH_SINGLE) ? QSPI_ADDRESS_1_LINE
                              : (cmd->address_mode == CFN_HAL_QSPI_BUS_WIDTH_DUAL) ? QSPI_ADDRESS_2_LINES
                              : (cmd->address_mode == CFN_HAL_QSPI_BUS_WIDTH_QUAD) ? QSPI_ADDRESS_4_LINES
                                                                                   : QSPI_ADDRESS_NONE;
    s_cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_cmd.DataMode          = (cmd->data_mode == CFN_HAL_QSPI_BUS_WIDTH_SINGLE) ? QSPI_DATA_1_LINE
                              : (cmd->data_mode == CFN_HAL_QSPI_BUS_WIDTH_DUAL) ? QSPI_DATA_2_LINES
                              : (cmd->data_mode == CFN_HAL_QSPI_BUS_WIDTH_QUAD) ? QSPI_DATA_4_LINES
                                                                                : QSPI_DATA_NONE;
    s_cmd.NbData            = cmd->nbr_of_data;
    s_cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    s_config.Match          = match;
    s_config.Mask           = mask;
    s_config.MatchMode      = QSPI_MATCH_MODE_AND;
    s_config.Interval       = 0x10;
    s_config.AutomaticStop  = QSPI_AUTOMATIC_STOP_ENABLE;

    return cfn_hal_stm32_map_error(HAL_QSPI_AutoPolling(&port_hqspis[port_id], &s_cmd, &s_config, timeout));
}

/* API --------------------------------------------------------------*/
static const cfn_hal_qspi_api_t QSPI_API = {
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
    .command = port_qspi_command,
    .transmit = port_qspi_transmit,
    .receive = port_qspi_receive,
    .memory_mapped_enable = port_qspi_memory_mapped_enable,
    .autopolling_enable = port_qspi_autopolling_enable,
};

#endif /* HAL_QSPI_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t cfn_hal_qspi_construct(cfn_hal_qspi_t              *driver,
                                            const cfn_hal_qspi_config_t *config,
                                            const cfn_hal_qspi_phy_t    *phy,
                                            struct cfn_hal_clock_s      *clock,
                                            void                        *dependency,
                                            cfn_hal_qspi_callback_t      callback,
                                            void                        *user_arg)
{
#ifdef HAL_QSPI_MODULE_ENABLED
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_QSPI_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t peripheral_id = PORT_MAP_PERIPHERAL_ID[port_id];
    cfn_hal_qspi_populate(driver, peripheral_id, clock, dependency, &QSPI_API, phy, config, callback, user_arg);

    port_hqspis[port_id].Instance = PORT_INSTANCES[port_id];
    port_drivers[port_id]         = driver;

    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    CFN_HAL_UNUSED(phy);
    CFN_HAL_UNUSED(clock);
    CFN_HAL_UNUSED(dependency);
    CFN_HAL_UNUSED(callback);
    CFN_HAL_UNUSED(user_arg);
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

    if (driver->phy != NULL)
    {
        uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
        if (port_id < CFN_HAL_QSPI_PORT_MAX)
        {
            port_drivers[port_id] = NULL;
        }
    }

    cfn_hal_qspi_populate(driver, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}
