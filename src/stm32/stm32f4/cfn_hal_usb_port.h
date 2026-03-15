/**
 * @file cfn_hal_usb_port.h
 * @brief STM32F4 USB HAL Port Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_USB_STM32F4_H
#define CAFFEINE_HAL_PORT_USB_STM32F4_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_usb.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Physical USB instances.
 */
typedef enum
{
    CFN_HAL_USB_PORT_OTG_FS,
    CFN_HAL_USB_PORT_OTG_HS,
    CFN_HAL_USB_PORT_MAX
} cfn_hal_usb_port_t;

/* Functions prototypes ---------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_usb_construct(cfn_hal_usb_t *driver, const cfn_hal_usb_config_t *config, const cfn_hal_usb_phy_t *phy);

cfn_hal_error_code_t cfn_hal_usb_destruct(cfn_hal_usb_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_USB_STM32F4_H */
