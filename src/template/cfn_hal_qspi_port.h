/**
 * @file cfn_hal_qspi_port.h
 * @brief QSPI HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_QSPI_H
#define CAFFEINE_HAL_PORT_QSPI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_qspi.h"

cfn_hal_error_code_t cfn_hal_qspi_construct(cfn_hal_qspi_t *driver, const cfn_hal_qspi_config_t *config, const cfn_hal_qspi_phy_t *phy);
cfn_hal_error_code_t cfn_hal_qspi_destruct(cfn_hal_qspi_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_QSPI_H */
