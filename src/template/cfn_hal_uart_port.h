/**
 * @file cfn_hal_uart_port.h
 * @brief UART HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_UART_H
#define CAFFEINE_HAL_PORT_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_uart.h"

/* Functions prototypes ---------------------------------------------*/

/**
 * @brief Construct the uart driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the configuration structure.
 * @param phy Pointer to the physical mapping structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_uart_construct(cfn_hal_uart_t *driver,
                                            const cfn_hal_uart_config_t *config,
                                            const cfn_hal_uart_phy_t *phy);

/**
 * @brief Destruct the uart driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_uart_destruct(cfn_hal_uart_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_UART_H */
