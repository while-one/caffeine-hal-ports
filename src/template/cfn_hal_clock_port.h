/**
 * @file cfn_hal_clock_port.h
 * @brief CLOCK HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_CLOCK_H
#define CAFFEINE_HAL_PORT_CLOCK_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_clock.h"

cfn_hal_error_code_t cfn_hal_clock_construct(cfn_hal_clock_t *driver, const cfn_hal_clock_config_t *config, const cfn_hal_clock_phy_t *phy);
cfn_hal_error_code_t cfn_hal_clock_destruct(cfn_hal_clock_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_CLOCK_H */
