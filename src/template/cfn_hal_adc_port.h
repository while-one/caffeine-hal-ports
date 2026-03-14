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

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_adc.h"

/* Functions prototypes ---------------------------------------------*/

/**
 * @brief Construct the adc driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the configuration structure.
 * @param phy Pointer to the physical mapping structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_adc_construct(cfn_hal_adc_t *driver,
                                            const cfn_hal_adc_config_t *config,
                                            const cfn_hal_adc_phy_t *phy);

/**
 * @brief Destruct the adc driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_adc_destruct(cfn_hal_adc_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_ADC_H */
