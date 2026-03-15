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

#include "cfn_hal_gpio.h"

cfn_hal_error_code_t cfn_hal_gpio_construct(cfn_hal_gpio_t *driver, void *config, const cfn_hal_gpio_phy_t *phy);
cfn_hal_error_code_t cfn_hal_gpio_destruct(cfn_hal_gpio_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_GPIO_H */
