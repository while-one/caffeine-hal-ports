/**
 * @file cfn_hal_spi_port.h
 * @brief STM32F4 SPI HAL Port Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_SPI_STM32F4_H
#define CAFFEINE_HAL_PORT_SPI_STM32F4_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_spi.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Physical SPI instances.
 */
typedef enum
{
    CFN_HAL_SPI_PORT_1,
    CFN_HAL_SPI_PORT_2,
    CFN_HAL_SPI_PORT_3,
    CFN_HAL_SPI_PORT_4,
    CFN_HAL_SPI_PORT_5,
    CFN_HAL_SPI_PORT_6,
    CFN_HAL_SPI_PORT_MAX
} cfn_hal_spi_port_t;

/* Functions prototypes ---------------------------------------------*/

cfn_hal_error_code_t cfn_hal_spi_construct(cfn_hal_spi_t *driver,
                                           const cfn_hal_spi_config_t *config,
                                           const cfn_hal_spi_phy_t *phy);

cfn_hal_error_code_t cfn_hal_spi_destruct(cfn_hal_spi_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_SPI_STM32F4_H */
