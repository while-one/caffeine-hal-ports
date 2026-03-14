/**
 * @file cfn_hal_comp_port.h
 * @brief COMP HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_COMP_H
#define CAFFEINE_HAL_PORT_COMP_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_comp.h"

/* Functions prototypes ---------------------------------------------*/

/**
 * @brief Construct the comp driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the configuration structure.
 * @param phy Pointer to the physical mapping structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t
cfn_hal_comp_construct(cfn_hal_comp_t *driver, const cfn_hal_comp_config_t *config, const cfn_hal_comp_phy_t *phy);

/**
 * @brief Destruct the comp driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_comp_destruct(cfn_hal_comp_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_COMP_H */
