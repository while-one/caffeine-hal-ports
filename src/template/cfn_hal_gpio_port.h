/**
 * @file cfn_hal_gpio_port.h
 * @brief GPIO HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_GPIO_H
#define CAFFEINE_HAL_PORT_GPIO_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_gpio.h"

/* Functions prototypes ---------------------------------------------*/

/**
 * @brief Construct the gpio driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the configuration structure.
 * @param phy Pointer to the physical mapping structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_gpio_construct(cfn_hal_gpio_t *driver,
                                            void *config,
                                            const cfn_hal_gpio_phy_t *phy);

/**
 * @brief Destruct the gpio driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_gpio_destruct(cfn_hal_gpio_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_GPIO_H */
