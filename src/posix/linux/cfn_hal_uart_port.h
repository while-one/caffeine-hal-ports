/**
 * @file cfn_hal_uart_port.h
 * @brief POSIX UART HAL Port Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_UART_POSIX_H
#define CAFFEINE_HAL_PORT_UART_POSIX_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_uart.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief POSIX Physical UART instances.
 */
typedef enum
{
    CFN_HAL_UART_PORT_1,
    CFN_HAL_UART_PORT_2,
    CFN_HAL_UART_PORT_MAX
} cfn_hal_uart_port_t;

/* Functions prototypes ---------------------------------------------*/

cfn_hal_error_code_t cfn_hal_uart_construct(cfn_hal_uart_t *driver,
                                            const cfn_hal_uart_config_t *config,
                                            const cfn_hal_uart_phy_t *phy);

cfn_hal_error_code_t cfn_hal_uart_destruct(cfn_hal_uart_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_UART_POSIX_H */
