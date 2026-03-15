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

#include "cfn_hal_wdt.h"

cfn_hal_error_code_t cfn_hal_wdt_construct(cfn_hal_wdt_t *driver, const cfn_hal_wdt_config_t *config, const cfn_hal_wdt_phy_t *phy);
cfn_hal_error_code_t cfn_hal_wdt_destruct(cfn_hal_wdt_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_WDT_H */
