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
 * @file cfn_hal_nvm_port.c
 * @brief STM32F4 NVM HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_nvm_port.h"
#include "cfn_hal_clock.h"
#include "cfn_hal_nvm.h"
#include "cfn_hal_stm32_error.h"
#include "stm32f4xx_hal.h"
#include <string.h>

/* Private Data -----------------------------------------------------*/
#define FLASH_BASE_ADDR 0x08000000U
#define FLASH_INVALID   0xFFFFFFFFU
#define SECTOR_COUNT    (sizeof(SECTOR_BOUNDARIES) / sizeof(SECTOR_BOUNDARIES[0]))
/* Internal Helpers -------------------------------------------------*/
/**
 * @brief STM32F4 Flash Sector Boundary Table.
 * Represents the end address of each sector.
 * Sectors 0-3: 16KB, Sector 4: 64KB, Sectors 5-11: 128KB.
 */
static const uint32_t SECTOR_BOUNDARIES[] = {
    0x08004000U, // Sector 0 (16 KB)
    0x08008000U, // Sector 1 (16 KB)
    0x0800C000U, // Sector 2 (16 KB)
    0x08010000U, // Sector 3 (16 KB)
    0x08020000U, // Sector 4 (64 KB)
    0x08040000U, // Sector 5 (128 KB)
    0x08060000U, // Sector 6 (128 KB)
    0x08080000U, // Sector 7 (128 KB)
    0x080A0000U, // Sector 8 (128 KB)
    0x080C0000U, // Sector 9 (128 KB)
    0x080E0000U, // Sector 10 (128 KB)
    0x08100000U  // Sector 11 (128 KB)
};

static const uint32_t PORT_MAP_SECTOR_SIZE[] = {
    16UL * 1024UL,  16UL * 1024UL,  16UL * 1024UL,  16UL * 1024UL,  // 0-3
    64UL * 1024UL,                                                  // 4
    128UL * 1024UL, 128UL * 1024UL, 128UL * 1024UL, 128UL * 1024UL, // 5-8
    128UL * 1024UL, 128UL * 1024UL, 128UL * 1024UL                  // 9-11
};

static uint32_t get_sector(uint32_t address)
{
    if ((address < FLASH_BASE_ADDR) || (address >= SECTOR_BOUNDARIES[SECTOR_COUNT - 1U]))
    {
        return FLASH_INVALID;
    }

    for (uint32_t i = 0U; i < SECTOR_COUNT; ++i)
    {
        if (address < SECTOR_BOUNDARIES[i])
        {
            return i;
        }
    }

    return FLASH_INVALID;
}

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t low_level_init(const cfn_hal_nvm_t *driver)
{
    if (driver == NULL || driver->phy == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    if (base == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    const cfn_hal_nvm_t *driver = (const cfn_hal_nvm_t *) base;

    return low_level_init(driver);
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
port_nvm_read(cfn_hal_nvm_t *driver, uint32_t addr, uint8_t *buffer, size_t size, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(timeout);
    memcpy(buffer, (const void *) (uintptr_t) addr, size); // NOLINT(performance-no-int-to-ptr)
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_nvm_write(cfn_hal_nvm_t *driver, uint32_t addr, const uint8_t *data, size_t size, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(timeout);
    HAL_StatusTypeDef status = HAL_OK;

    HAL_FLASH_Unlock();

    for (size_t i = 0; i < size; i++)
    {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr + i, (uint64_t) data[i]);
        if (status != HAL_OK)
        {
            break;
        }
    }

    HAL_FLASH_Lock();

    return cfn_hal_stm32_map_error(status);
}

static cfn_hal_error_code_t port_nvm_erase_sector(cfn_hal_nvm_t *driver, uint32_t sector_addr, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(timeout);
    FLASH_EraseInitTypeDef erase_init   = { 0 };
    uint32_t               sector_error = 0;
    HAL_StatusTypeDef      status       = HAL_OK;

    HAL_FLASH_Unlock();

    erase_init.TypeErase    = FLASH_TYPEERASE_SECTORS;
    erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    erase_init.Sector       = get_sector(sector_addr);
    erase_init.NbSectors    = 1;

    status                  = HAL_FLASHEx_Erase(&erase_init, &sector_error);

    HAL_FLASH_Lock();

    return cfn_hal_stm32_map_error(status);
}

static cfn_hal_error_code_t port_nvm_erase_chip(cfn_hal_nvm_t *driver, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(timeout);
    FLASH_EraseInitTypeDef erase_init   = { 0 };
    uint32_t               sector_error = 0;
    HAL_StatusTypeDef      status       = HAL_OK;

    HAL_FLASH_Unlock();

    erase_init.TypeErase    = FLASH_TYPEERASE_MASSERASE;
    erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    status                  = HAL_FLASHEx_Erase(&erase_init, &sector_error);

    HAL_FLASH_Lock();

    return cfn_hal_stm32_map_error(status);
}

static cfn_hal_error_code_t port_nvm_get_info(cfn_hal_nvm_t *driver, uint32_t addr, cfn_hal_nvm_info_t *info)
{
    CFN_HAL_UNUSED(driver);
    if (info == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t sector = get_sector(addr);
    if (sector == 0xFFFFFFFF)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /* FLASHSIZE_BASE points to a 16-bit register containing flash size in KB */
    info->total_size   = (*((uint16_t *) FLASHSIZE_BASE)) * 1024UL;
    info->page_size    = 1; /* Byte-programmable */
    info->write_cycles = 10000;
    info->sector_size  = PORT_MAP_SECTOR_SIZE[sector];

    return CFN_HAL_ERROR_OK;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_nvm_api_t NVM_API = {
    .base =
        {
            .init = port_base_init,
            .deinit = NULL,
            .power_state_set = NULL,
            .config_set = NULL,
            .config_validate = NULL,
            .callback_register = NULL,
            .event_enable = NULL,
            .event_disable = NULL,
            .event_get = port_base_event_get,
            .error_enable = NULL,
            .error_disable = NULL,
            .error_get = port_base_error_get,
        },
    .read = port_nvm_read,
    .write = port_nvm_write,
    .erase_sector = port_nvm_erase_sector,
    .erase_chip = port_nvm_erase_chip,
    .get_info = port_nvm_get_info};

/* Instantiation ----------------------------------------------------*/
cfn_hal_error_code_t cfn_hal_nvm_construct(cfn_hal_nvm_t              *driver,
                                           const cfn_hal_nvm_config_t *config,
                                           const cfn_hal_nvm_phy_t    *phy,
                                           struct cfn_hal_clock_s     *clock,
                                           void                       *dependency,
                                           cfn_hal_nvm_callback_t      callback,
                                           void                       *user_arg)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    cfn_hal_nvm_populate(driver, 0, clock, dependency, &NVM_API, phy, config, callback, user_arg);

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_nvm_destruct(cfn_hal_nvm_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_nvm_populate(driver, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    return CFN_HAL_ERROR_OK;
}
