/**
 * @file cfn_hal_i2c_port.h
 * @brief STM32F4 I2C HAL Port Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_I2C_STM32F4_H
#define CAFFEINE_HAL_PORT_I2C_STM32F4_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_i2c.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Physical I2C instances.
 */
typedef enum
{
    CFN_HAL_I2C_PORT_I2C1,
    CFN_HAL_I2C_PORT_I2C2,
    CFN_HAL_I2C_PORT_I2C3,
    CFN_HAL_I2C_PORT_MAX
} cfn_hal_i2c_port_t;

/* Functions prototypes ---------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_i2c_construct(cfn_hal_i2c_t *driver, const cfn_hal_i2c_config_t *config, const cfn_hal_i2c_phy_t *phy);

cfn_hal_error_code_t cfn_hal_i2c_destruct(cfn_hal_i2c_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_I2C_STM32F4_H */
