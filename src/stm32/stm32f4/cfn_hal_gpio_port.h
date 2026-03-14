/**
 * @file cfn_hal_gpio_port.h
 * @brief STM32F4 GPIO HAL Port Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_GPIO_STM32F4_H
#define CAFFEINE_HAL_PORT_GPIO_STM32F4_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_gpio.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Physical GPIO Ports.
 */
typedef enum
{
    CFN_HAL_GPIO_PORT_A,
    CFN_HAL_GPIO_PORT_B,
    CFN_HAL_GPIO_PORT_C,
    CFN_HAL_GPIO_PORT_D,
    CFN_HAL_GPIO_PORT_E,
    CFN_HAL_GPIO_PORT_F,
    CFN_HAL_GPIO_PORT_G,
    CFN_HAL_GPIO_PORT_H,
    CFN_HAL_GPIO_PORT_I,
    CFN_HAL_GPIO_PORT_J,
    CFN_HAL_GPIO_PORT_K,
    CFN_HAL_GPIO_PORT_MAX
} cfn_hal_gpio_port_t;

/* Functions prototypes ---------------------------------------------*/

cfn_hal_error_code_t cfn_hal_gpio_construct(cfn_hal_gpio_t *driver,
                                            void *config,
                                            const cfn_hal_gpio_phy_t *phy);

cfn_hal_error_code_t cfn_hal_gpio_destruct(cfn_hal_gpio_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_GPIO_STM32F4_H */
