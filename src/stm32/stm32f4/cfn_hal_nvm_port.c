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

/* Internal Helpers -------------------------------------------------*/

static uint32_t get_sector(uint32_t address) {
  uint32_t sector = 0;

  if ((address < 0x08004000) && (address >= 0x08000000)) {
    sector = FLASH_SECTOR_0;
  } else if ((address < 0x08008000) && (address >= 0x08004000)) {
    sector = FLASH_SECTOR_1;
  } else if ((address < 0x0800C000) && (address >= 0x08008000)) {
    sector = FLASH_SECTOR_2;
  } else if ((address < 0x08010000) && (address >= 0x0800C000)) {
    sector = FLASH_SECTOR_3;
  } else if ((address < 0x08020000) && (address >= 0x08010000)) {
    sector = FLASH_SECTOR_4;
  } else if ((address < 0x08040000) && (address >= 0x08020000)) {
    sector = FLASH_SECTOR_5;
  } else if ((address < 0x08060000) && (address >= 0x08040000)) {
    sector = FLASH_SECTOR_6;
  } else if ((address < 0x08080000) && (address >= 0x08060000)) {
    sector = FLASH_SECTOR_7;
  } else if ((address < 0x080A0000) && (address >= 0x08080000)) {
    sector = FLASH_SECTOR_8;
  } else if ((address < 0x080C0000) && (address >= 0x080A0000)) {
    sector = FLASH_SECTOR_9;
  } else if ((address < 0x080E0000) && (address >= 0x080C0000)) {
    sector = FLASH_SECTOR_10;
  } else if ((address < 0x08100000) && (address >= 0x080E0000)) {
    sector = FLASH_SECTOR_11;
  } else {
    /* Address out of range or beyond 1MB bank */
    sector = 0xFFFFFFFF;
  }

  return sector;
}

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t low_level_init(cfn_hal_nvm_t *driver) {
  if (driver == NULL || driver->phy == NULL) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base) {
  if (base == NULL) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }
  cfn_hal_nvm_t *driver = (cfn_hal_nvm_t *)base;

  return low_level_init(driver);
}

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base,
                                                uint32_t *event_mask) {
  CFN_HAL_UNUSED(base);
  if (event_mask != NULL) {
    *event_mask = 0;
  }
  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base,
                                                uint32_t *error_mask) {
  CFN_HAL_UNUSED(base);
  if (error_mask != NULL) {
    *error_mask = 0;
  }
  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_nvm_read(cfn_hal_nvm_t *driver, uint32_t addr,
                                          uint8_t *buffer, size_t size) {
  CFN_HAL_UNUSED(driver);
  memcpy(buffer, (const void *)(uintptr_t)addr,
         size); // NOLINT(performance-no-int-to-ptr)
  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_nvm_write(cfn_hal_nvm_t *driver, uint32_t addr,
                                           const uint8_t *data, size_t size) {
  CFN_HAL_UNUSED(driver);
  HAL_StatusTypeDef status = HAL_OK;

  HAL_FLASH_Unlock();

  for (size_t i = 0; i < size; i++) {
    status =
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr + i, (uint64_t)data[i]);
    if (status != HAL_OK) {
      break;
    }
  }

  HAL_FLASH_Lock();

  return cfn_hal_stm32_map_error(status);
}

static cfn_hal_error_code_t port_nvm_erase_sector(cfn_hal_nvm_t *driver,
                                                  uint32_t sector_addr) {
  CFN_HAL_UNUSED(driver);
  FLASH_EraseInitTypeDef erase_init = {0};
  uint32_t sector_error = 0;
  HAL_StatusTypeDef status = HAL_OK;

  HAL_FLASH_Unlock();

  erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
  erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  erase_init.Sector = get_sector(sector_addr);
  erase_init.NbSectors = 1;

  status = HAL_FLASHEx_Erase(&erase_init, &sector_error);

  HAL_FLASH_Lock();

  return cfn_hal_stm32_map_error(status);
}

static cfn_hal_error_code_t port_nvm_erase_chip(cfn_hal_nvm_t *driver) {
  CFN_HAL_UNUSED(driver);
  FLASH_EraseInitTypeDef erase_init = {0};
  uint32_t sector_error = 0;
  HAL_StatusTypeDef status = HAL_OK;

  HAL_FLASH_Unlock();

  erase_init.TypeErase = FLASH_TYPEERASE_MASSERASE;
  erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;

  status = HAL_FLASHEx_Erase(&erase_init, &sector_error);

  HAL_FLASH_Lock();

  return cfn_hal_stm32_map_error(status);
}

static cfn_hal_error_code_t port_nvm_get_info(cfn_hal_nvm_t *driver,
                                              cfn_hal_nvm_info_t *info) {
  CFN_HAL_UNUSED(driver);
  if (info == NULL) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  info->total_size = 1024 * 1024; /* Assume 1MB for F417VG */
  info->sector_size = 128 * 1024; /* Large sectors on F4 */
  info->page_size = 1;            /* Byte-programmable */
  info->write_cycles = 10000;

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
cfn_hal_error_code_t cfn_hal_nvm_construct(cfn_hal_nvm_t *driver,
                                           const cfn_hal_nvm_config_t *config,
                                           const cfn_hal_nvm_phy_t *phy,
                                           struct cfn_hal_clock_s *clock,
                                           cfn_hal_nvm_callback_t callback,
                                           void *user_arg) {
  if ((driver == NULL) || (phy == NULL)) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  cfn_hal_nvm_populate(driver, 0, clock, &NVM_API, phy, config, callback,
                       user_arg);

  return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_nvm_destruct(cfn_hal_nvm_t *driver) {
  if (driver == NULL) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }
  driver->config = NULL;
  driver->phy = NULL;

  return CFN_HAL_ERROR_OK;
}
