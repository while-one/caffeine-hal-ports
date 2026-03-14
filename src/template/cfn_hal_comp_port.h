/**
 * @file cfn_hal_comp_port.h
 * @brief COMP HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_COMP_H
#define CAFFEINE_HAL_PORT_COMP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_comp.h"

cfn_hal_error_code_t cfn_hal_comp_construct(cfn_hal_comp_t *driver, const cfn_hal_comp_config_t *config, const cfn_hal_comp_phy_t *phy);
cfn_hal_error_code_t cfn_hal_comp_destruct(cfn_hal_comp_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_COMP_H */
