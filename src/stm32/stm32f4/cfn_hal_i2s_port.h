/**
 * @file cfn_hal_i2s_port.h
 * @brief STM32F4 I2S HAL Port Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_I2S_STM32F4_H
#define CAFFEINE_HAL_PORT_I2S_STM32F4_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_i2s.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Physical I2S instances.
 */
typedef enum
{
    CFN_HAL_I2S_PORT_2,
    CFN_HAL_I2S_PORT_3,
    CFN_HAL_I2S_PORT_MAX
} cfn_hal_i2s_port_t;

/* Functions prototypes ---------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_i2s_construct(cfn_hal_i2s_t *driver, const cfn_hal_i2s_config_t *config, const cfn_hal_i2s_phy_t *phy);

cfn_hal_error_code_t cfn_hal_i2s_destruct(cfn_hal_i2s_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_I2S_STM32F4_H */
