/**
 * @file cfn_hal_wdt_port.h
 * @brief WDT HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_WDT_H
#define CAFFEINE_HAL_PORT_WDT_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_wdt.h"

/* Functions prototypes ---------------------------------------------*/

/**
 * @brief Construct the wdt driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the configuration structure.
 * @param phy Pointer to the physical mapping structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t
cfn_hal_wdt_construct(cfn_hal_wdt_t *driver, const cfn_hal_wdt_config_t *config, const cfn_hal_wdt_phy_t *phy);

/**
 * @brief Destruct the wdt driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_wdt_destruct(cfn_hal_wdt_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_WDT_H */
