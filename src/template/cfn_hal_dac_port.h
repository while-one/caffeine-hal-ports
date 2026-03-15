/**
 * @file cfn_hal_dac_port.h
 * @brief DAC HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_DAC_H
#define CAFFEINE_HAL_PORT_DAC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_dac.h"

cfn_hal_error_code_t cfn_hal_dac_construct(cfn_hal_dac_t *driver, const cfn_hal_dac_config_t *config, const cfn_hal_dac_phy_t *phy);
cfn_hal_error_code_t cfn_hal_dac_destruct(cfn_hal_dac_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_DAC_H */
