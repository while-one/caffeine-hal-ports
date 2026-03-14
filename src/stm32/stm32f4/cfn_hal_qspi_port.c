/**
 * @file cfn_hal_qspi_port.c
 * @brief STM32F4 QSPI HAL Port Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_qspi.h"
#include "cfn_hal_qspi_port.h"
#include "cfn_hal_stm32_error.h"

#if defined(QUADSPI)

/* Private Data -----------------------------------------------------*/

static QSPI_HandleTypeDef port_hqspi;

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    CFN_HAL_UNUSED(base);

    __HAL_RCC_QSPI_CLK_ENABLE();

    port_hqspi.Instance = QUADSPI;
    port_hqspi.Init.ClockPrescaler = 1;
    port_hqspi.Init.FifoThreshold = 4;
    port_hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    port_hqspi.Init.FlashSize = 23; /* 16MB */
    port_hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
    port_hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
    port_hqspi.Init.FlashID = QSPI_FLASH_ID_1;
    port_hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;

    return cfn_hal_stm32_map_error(HAL_QSPI_Init(&port_hqspi));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    CFN_HAL_UNUSED(base);
    return cfn_hal_stm32_map_error(HAL_QSPI_DeInit(&port_hqspi));
}

static cfn_hal_error_code_t port_base_power_state_set(cfn_hal_driver_t *base, cfn_hal_power_state_t state)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(state);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
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
    if (event_mask != NULL)
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

    s_cmd.Instruction = cmd->instruction;
    s_cmd.Address = cmd->address;
    s_cmd.AlternateBytes = cmd->alternate_bytes;
    s_cmd.AddressSize = cmd->address_size;
    s_cmd.AlternateBytesSize = cmd->alternate_bytes_size;
    s_cmd.DummyCycles = cmd->dummy_cycles;
    s_cmd.InstructionMode = cmd->instruction_mode;
    s_cmd.AddressMode = cmd->address_mode;
    s_cmd.AlternateByteMode = cmd->alternate_bytes_mode;
    s_cmd.DataMode = cmd->data_mode;
    s_cmd.NbData = cmd->nb_data;
    s_cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

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
    QSPI_CommandTypeDef      s_cmd = { 0 };
    QSPI_MemoryMappedTypeDef s_mem_mapped = { 0 };

    s_cmd.Instruction = cmd->instruction;
    s_cmd.Address = cmd->address;
    s_cmd.AlternateBytes = cmd->alternate_bytes;
    s_cmd.AddressSize = cmd->address_size;
    s_cmd.AlternateBytesSize = cmd->alternate_bytes_size;
    s_cmd.DummyCycles = cmd->dummy_cycles;
    s_cmd.InstructionMode = cmd->instruction_mode;
    s_cmd.AddressMode = cmd->address_mode;
    s_cmd.AlternateByteMode = cmd->alternate_bytes_mode;
    s_cmd.DataMode = cmd->data_mode;
    s_cmd.NbData = cmd->nb_data;
    s_cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    s_mem_mapped.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;

    return cfn_hal_stm32_map_error(HAL_QSPI_MemoryMapped(&port_hqspi, &s_cmd, &s_mem_mapped));
}

/* API --------------------------------------------------------------*/
static const cfn_hal_qspi_api_t QSPI_API = {
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
cfn_hal_error_code_t
cfn_hal_qspi_construct(cfn_hal_qspi_t *driver, const cfn_hal_qspi_config_t *config, const cfn_hal_qspi_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &QSPI_API;
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

#else

/* Stub implementation for variants without QUADSPI */

cfn_hal_error_code_t
cfn_hal_qspi_construct(cfn_hal_qspi_t *driver, const cfn_hal_qspi_config_t *config, const cfn_hal_qspi_phy_t *phy)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    CFN_HAL_UNUSED(phy);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

cfn_hal_error_code_t cfn_hal_qspi_destruct(cfn_hal_qspi_t *driver)
{
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

#endif
