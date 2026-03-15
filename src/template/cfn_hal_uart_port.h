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

#include "cfn_hal_uart.h"

cfn_hal_error_code_t cfn_hal_uart_construct(cfn_hal_uart_t *driver, const cfn_hal_uart_config_t *config, const cfn_hal_uart_phy_t *phy);
cfn_hal_error_code_t cfn_hal_uart_destruct(cfn_hal_uart_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_UART_H */
