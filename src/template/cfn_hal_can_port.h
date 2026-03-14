/**
 * @file cfn_hal_can_port.h
 * @brief CAN HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_CAN_H
#define CAFFEINE_HAL_PORT_CAN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_can.h"

cfn_hal_error_code_t cfn_hal_can_construct(cfn_hal_can_t *driver, const cfn_hal_can_config_t *config, const cfn_hal_can_phy_t *phy);
cfn_hal_error_code_t cfn_hal_can_destruct(cfn_hal_can_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_CAN_H */
