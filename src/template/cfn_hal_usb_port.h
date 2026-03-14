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

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_usb.h"

/* Functions prototypes ---------------------------------------------*/

/**
 * @brief Construct the usb driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the configuration structure.
 * @param phy Pointer to the physical mapping structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_usb_construct(cfn_hal_usb_t *driver,
                                            const cfn_hal_usb_config_t *config,
                                            const cfn_hal_usb_phy_t *phy);

/**
 * @brief Destruct the usb driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_usb_destruct(cfn_hal_usb_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_USB_H */
