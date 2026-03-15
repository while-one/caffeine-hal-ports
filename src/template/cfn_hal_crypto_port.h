/**
 * @file cfn_hal_crypto_port.h
 * @brief CRYPTO HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_CRYPTO_H
#define CAFFEINE_HAL_PORT_CRYPTO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_crypto.h"

cfn_hal_error_code_t cfn_hal_crypto_construct(cfn_hal_crypto_t *driver, const cfn_hal_crypto_config_t *config, const cfn_hal_crypto_phy_t *phy);
cfn_hal_error_code_t cfn_hal_crypto_destruct(cfn_hal_crypto_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_CRYPTO_H */
