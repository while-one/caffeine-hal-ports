/**
 * @file cfn_hal_adc_port.h
 * @brief STM32F4 ADC HAL Port Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_ADC_STM32F4_H
#define CAFFEINE_HAL_PORT_ADC_STM32F4_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_adc.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Physical ADC instances.
 */
typedef enum
{
    CFN_HAL_ADC_PORT_ADC1,
    CFN_HAL_ADC_PORT_ADC2,
    CFN_HAL_ADC_PORT_ADC3,
    CFN_HAL_ADC_PORT_MAX
} cfn_hal_adc_port_t;

/* Functions prototypes ---------------------------------------------*/

cfn_hal_error_code_t cfn_hal_adc_construct(cfn_hal_adc_t *driver,
                                           const cfn_hal_adc_config_t *config,
                                           const cfn_hal_adc_phy_t *phy);

cfn_hal_error_code_t cfn_hal_adc_destruct(cfn_hal_adc_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_ADC_STM32F4_H */
