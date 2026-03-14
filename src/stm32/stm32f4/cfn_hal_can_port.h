/**
 * @file cfn_hal_can_port.h
 * @brief STM32F4 CAN HAL Port Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_CAN_STM32F4_H
#define CAFFEINE_HAL_PORT_CAN_STM32F4_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_can.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Physical CAN instances.
 */
typedef enum
{
    CFN_HAL_CAN_PORT_CAN1,
    CFN_HAL_CAN_PORT_CAN2,
#if defined(CAN3)
    CFN_HAL_CAN_PORT_CAN3,
#endif
    CFN_HAL_CAN_PORT_MAX
} cfn_hal_can_port_t;

/* Functions prototypes ---------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_can_construct(cfn_hal_can_t *driver, const cfn_hal_can_config_t *config, const cfn_hal_can_phy_t *phy);

cfn_hal_error_code_t cfn_hal_can_destruct(cfn_hal_can_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_CAN_STM32F4_H */
