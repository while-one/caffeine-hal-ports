/**
 * @file cfn_hal_dac_port.h
 * @brief STM32F4 DAC HAL Port Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_DAC_STM32F4_H
#define CAFFEINE_HAL_PORT_DAC_STM32F4_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_dac.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Physical DAC instances.
 */
typedef enum
{
    CFN_HAL_DAC_PORT_DAC1,
    CFN_HAL_DAC_PORT_MAX
} cfn_hal_dac_port_t;

/* Functions prototypes ---------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_dac_construct(cfn_hal_dac_t *driver, const cfn_hal_dac_config_t *config, const cfn_hal_dac_phy_t *phy);

cfn_hal_error_code_t cfn_hal_dac_destruct(cfn_hal_dac_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_DAC_STM32F4_H */
