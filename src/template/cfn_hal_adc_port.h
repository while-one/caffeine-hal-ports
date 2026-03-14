/**
 * @file cfn_hal_adc_port.h
 * @brief ADC HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_ADC_H
#define CAFFEINE_HAL_PORT_ADC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_adc.h"

cfn_hal_error_code_t cfn_hal_adc_construct(cfn_hal_adc_t *driver, const cfn_hal_adc_config_t *config, const cfn_hal_adc_phy_t *phy);
cfn_hal_error_code_t cfn_hal_adc_destruct(cfn_hal_adc_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_ADC_H */
