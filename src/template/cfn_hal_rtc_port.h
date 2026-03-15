/**
 * @file cfn_hal_rtc_port.h
 * @brief RTC HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_RTC_H
#define CAFFEINE_HAL_PORT_RTC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_rtc.h"

cfn_hal_error_code_t cfn_hal_rtc_construct(cfn_hal_rtc_t *driver, const cfn_hal_rtc_config_t *config, const cfn_hal_rtc_phy_t *phy);
cfn_hal_error_code_t cfn_hal_rtc_destruct(cfn_hal_rtc_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_RTC_H */
