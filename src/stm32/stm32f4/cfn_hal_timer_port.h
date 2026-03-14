/**
 * @file cfn_hal_timer_port.h
 * @brief STM32F4 Timer HAL Port Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_TIMER_STM32F4_H
#define CAFFEINE_HAL_PORT_TIMER_STM32F4_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_timer.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Physical Timer instances.
 */
typedef enum
{
    CFN_HAL_TIMER_PORT_TIM1,
    CFN_HAL_TIMER_PORT_TIM2,
    CFN_HAL_TIMER_PORT_TIM3,
    CFN_HAL_TIMER_PORT_TIM4,
    CFN_HAL_TIMER_PORT_TIM5,
    CFN_HAL_TIMER_PORT_TIM6,
    CFN_HAL_TIMER_PORT_TIM7,
    CFN_HAL_TIMER_PORT_TIM8,
    CFN_HAL_TIMER_PORT_TIM9,
    CFN_HAL_TIMER_PORT_TIM10,
    CFN_HAL_TIMER_PORT_TIM11,
    CFN_HAL_TIMER_PORT_TIM12,
    CFN_HAL_TIMER_PORT_TIM13,
    CFN_HAL_TIMER_PORT_TIM14,
    CFN_HAL_TIMER_PORT_MAX
} cfn_hal_timer_port_t;

/* Functions prototypes ---------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_timer_construct(cfn_hal_timer_t *driver, const cfn_hal_timer_config_t *config, const cfn_hal_timer_phy_t *phy);

cfn_hal_error_code_t cfn_hal_timer_destruct(cfn_hal_timer_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_TIMER_STM32F4_H */
