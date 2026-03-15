/**
 * @file cfn_hal_usb_port.h
 * @brief USB HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_USB_H
#define CAFFEINE_HAL_PORT_USB_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_usb.h"

cfn_hal_error_code_t cfn_hal_usb_construct(cfn_hal_usb_t *driver, const cfn_hal_usb_config_t *config, const cfn_hal_usb_phy_t *phy);
cfn_hal_error_code_t cfn_hal_usb_destruct(cfn_hal_usb_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_USB_H */
