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
 * @file cfn_hal_wdt_port.c
 * @brief STM32F4 WDT HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_wdt_port.h"
#include "cfn_hal_clock.h"
#include "cfn_hal_stm32_error.h"
#include "cfn_hal_wdt.h"
#include "stm32f4xx_hal.h"

#if defined(HAL_IWDG_MODULE_ENABLED) || defined(HAL_WWDG_MODULE_ENABLED)

/* Private Data -----------------------------------------------------*/

/* WDT enums usually not in cfn_hal_wdt.h because it's config-based,
   but we'll assume a single instance for STM32F4 IWDG */
#define CFN_HAL_WDT_PORT_IWDG 0
#define CFN_HAL_WDT_PORT_MAX 1

static IWDG_TypeDef *const PORT_INSTANCES[CFN_HAL_WDT_PORT_MAX] = {
#if defined(IWDG)
    [CFN_HAL_WDT_PORT_IWDG] = IWDG,
#endif
};

static const uint32_t PORT_MAP_PERIPHERAL_ID[CFN_HAL_WDT_PORT_MAX] = {
    [CFN_HAL_WDT_PORT_IWDG] = 0,
};

static IWDG_HandleTypeDef port_hiwdgs[CFN_HAL_WDT_PORT_MAX];

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t low_level_init(cfn_hal_wdt_t *driver) {
  if (driver == NULL || driver->phy == NULL) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;

  if (port_id >= CFN_HAL_WDT_PORT_MAX) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base) {
  if (base == NULL) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }
  cfn_hal_wdt_t *driver = (cfn_hal_wdt_t *)base;
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  IWDG_HandleTypeDef *hiwdg = &port_hiwdgs[port_id];

  cfn_hal_error_code_t error = low_level_init(driver);
  if (error != CFN_HAL_ERROR_OK) {
    return error;
  }

  hiwdg->Instance = PORT_INSTANCES[port_id];
  hiwdg->Init.Prescaler = IWDG_PRESCALER_4;
  hiwdg->Init.Reload = 4095;

  return cfn_hal_stm32_map_error(HAL_IWDG_Init(hiwdg));
}

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base,
                                                 const void *config) {
  CFN_HAL_UNUSED(config);
  return port_base_init(base);
}

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base,
                                                uint32_t *event_mask) {
  CFN_HAL_UNUSED(base);
  if (event_mask) {
    *event_mask = 0;
  }
  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base,
                                                uint32_t *error_mask) {
  CFN_HAL_UNUSED(base);
  if (error_mask) {
    *error_mask = 0;
  }
  return CFN_HAL_ERROR_OK;
}

/* WDT Specific Functions */

static cfn_hal_error_code_t port_wdt_feed(cfn_hal_wdt_t *driver) {
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  return cfn_hal_stm32_map_error(HAL_IWDG_Refresh(&port_hiwdgs[port_id]));
}

/* API --------------------------------------------------------------*/
static const cfn_hal_wdt_api_t WDT_API = {
    .base =
        {
            .init = port_base_init,
            .deinit = NULL,
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
    .start = NULL,
    .stop = NULL,
    .feed = port_wdt_feed};

#endif /* HAL_IWDG_MODULE_ENABLED || HAL_WWDG_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/

/**
 * @brief Constructs a new WDT driver instance.
 * @param driver Pointer to the driver structure to construct.
 * @param config Pointer to the WDT configuration.
 * @param phy Pointer to the physical WDT mapping.
 * @param clock Pointer to the clock driver instance.
 * @param callback User callback function.
 * @param user_arg User argument for the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_wdt_construct(cfn_hal_wdt_t *driver,
                                           const cfn_hal_wdt_config_t *config,
                                           const cfn_hal_wdt_phy_t *phy,
                                           struct cfn_hal_clock_s *clock,
                                           cfn_hal_wdt_callback_t callback,
                                           void *user_arg) {
#if defined(HAL_IWDG_MODULE_ENABLED) || defined(HAL_WWDG_MODULE_ENABLED)
  if ((driver == NULL) || (phy == NULL)) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  uint32_t port_id = (uint32_t)(uintptr_t)phy->instance;
  if (port_id >= CFN_HAL_WDT_PORT_MAX || PORT_INSTANCES[port_id] == NULL) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  uint32_t peripheral_id = PORT_MAP_PERIPHERAL_ID[port_id];
  cfn_hal_wdt_populate(driver, peripheral_id, clock, &WDT_API, phy, config,
                       callback, user_arg);

  port_hiwdgs[port_id].Instance = PORT_INSTANCES[port_id];

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

/**
 * @brief Destructs an existing WDT driver instance.
 * @param driver Pointer to the driver instance to destruct.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_wdt_destruct(cfn_hal_wdt_t *driver) {
#if defined(HAL_IWDG_MODULE_ENABLED) || defined(HAL_WWDG_MODULE_ENABLED)
  if (driver == NULL) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  driver->config = NULL;
  driver->phy = NULL;
  return CFN_HAL_ERROR_OK;
#else
  CFN_HAL_UNUSED(driver);
  return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}
