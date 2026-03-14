/**
 * @file cfn_hal_rtc_port.h
 * @brief STM32F4 RTC HAL Port Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_RTC_STM32F4_H
#define CAFFEINE_HAL_PORT_RTC_STM32F4_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_rtc.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Physical RTC instances.
 */
typedef enum
{
    CFN_HAL_RTC_PORT_RTC1,
    CFN_HAL_RTC_PORT_MAX
} cfn_hal_rtc_port_t;

/* Functions prototypes ---------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_rtc_construct(cfn_hal_rtc_t *driver, const cfn_hal_rtc_config_t *config, const cfn_hal_rtc_phy_t *phy);

cfn_hal_error_code_t cfn_hal_rtc_destruct(cfn_hal_rtc_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_RTC_STM32F4_H */
