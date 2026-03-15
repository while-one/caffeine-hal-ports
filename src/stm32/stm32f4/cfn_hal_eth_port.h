/**
 * @file cfn_hal_eth_port.h
 * @brief STM32F4 ETH HAL Port Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_ETH_STM32F4_H
#define CAFFEINE_HAL_PORT_ETH_STM32F4_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_eth.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Physical ETH instances.
 */
typedef enum
{
    CFN_HAL_ETH_PORT_ETH1,
    CFN_HAL_ETH_PORT_MAX
} cfn_hal_eth_port_t;

/* Functions prototypes ---------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_eth_construct(cfn_hal_eth_t *driver, const cfn_hal_eth_config_t *config, const cfn_hal_eth_phy_t *phy);

cfn_hal_error_code_t cfn_hal_eth_destruct(cfn_hal_eth_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_ETH_STM32F4_H */
