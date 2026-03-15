/**
 * @file cfn_hal_spi_port.h
 * @brief SPI HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_SPI_H
#define CAFFEINE_HAL_PORT_SPI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_spi.h"

cfn_hal_error_code_t cfn_hal_spi_construct(cfn_hal_spi_t *driver, const cfn_hal_spi_config_t *config, const cfn_hal_spi_phy_t *phy);
cfn_hal_error_code_t cfn_hal_spi_destruct(cfn_hal_spi_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_SPI_H */
