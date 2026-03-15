/**
 * @file cfn_hal_eth_port.h
 * @brief ETH HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_ETH_H
#define CAFFEINE_HAL_PORT_ETH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_eth.h"

cfn_hal_error_code_t cfn_hal_eth_construct(cfn_hal_eth_t *driver, const cfn_hal_eth_config_t *config, const cfn_hal_eth_phy_t *phy);
cfn_hal_error_code_t cfn_hal_eth_destruct(cfn_hal_eth_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_ETH_H */
