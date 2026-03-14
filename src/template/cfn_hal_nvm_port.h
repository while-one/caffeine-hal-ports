/**
 * @file cfn_hal_nvm_port.h
 * @brief NVM HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_NVM_H
#define CAFFEINE_HAL_PORT_NVM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_nvm.h"

cfn_hal_error_code_t cfn_hal_nvm_construct(cfn_hal_nvm_t *driver, const cfn_hal_nvm_config_t *config, const cfn_hal_nvm_phy_t *phy);
cfn_hal_error_code_t cfn_hal_nvm_destruct(cfn_hal_nvm_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_NVM_H */
