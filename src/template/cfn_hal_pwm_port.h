/**
 * @file cfn_hal_pwm_port.h
 * @brief PWM HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_PWM_H
#define CAFFEINE_HAL_PORT_PWM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_pwm.h"

cfn_hal_error_code_t cfn_hal_pwm_construct(cfn_hal_pwm_t *driver, const cfn_hal_pwm_config_t *config, const cfn_hal_pwm_phy_t *phy);
cfn_hal_error_code_t cfn_hal_pwm_destruct(cfn_hal_pwm_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_PWM_H */
