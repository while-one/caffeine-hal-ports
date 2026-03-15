/**
 * @file cfn_hal_sdio_port.h
 * @brief SDIO HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_SDIO_H
#define CAFFEINE_HAL_PORT_SDIO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_sdio.h"

cfn_hal_error_code_t cfn_hal_sdio_construct(cfn_hal_sdio_t *driver, const cfn_hal_sdio_config_t *config, const cfn_hal_sdio_phy_t *phy);
cfn_hal_error_code_t cfn_hal_sdio_destruct(cfn_hal_sdio_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_SDIO_H */
