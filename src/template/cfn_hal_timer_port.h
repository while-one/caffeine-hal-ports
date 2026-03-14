/**
 * @file cfn_hal_timer_port.h
 * @brief TIMER HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_TIMER_H
#define CAFFEINE_HAL_PORT_TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_timer.h"

/* Functions prototypes ---------------------------------------------*/

/**
 * @brief Construct the timer driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the configuration structure.
 * @param phy Pointer to the physical mapping structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_timer_construct(cfn_hal_timer_t *driver,
                                            const cfn_hal_timer_config_t *config,
                                            const cfn_hal_timer_phy_t *phy);

/**
 * @brief Destruct the timer driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_timer_destruct(cfn_hal_timer_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_TIMER_H */
