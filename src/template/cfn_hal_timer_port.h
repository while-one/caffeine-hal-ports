/**
 * @file cfn_hal_timer_port.h
 * @brief TIMER HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_TIMER_H
#define CAFFEINE_HAL_PORT_TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_timer.h"

cfn_hal_error_code_t cfn_hal_timer_construct(cfn_hal_timer_t *driver, const cfn_hal_timer_config_t *config, const cfn_hal_timer_phy_t *phy);
cfn_hal_error_code_t cfn_hal_timer_destruct(cfn_hal_timer_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_TIMER_H */
