/**
 * @file cfn_hal_uart_port.h
 * @brief STM32F4 UART HAL Port Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_UART_STM32F4_H
#define CAFFEINE_HAL_PORT_UART_STM32F4_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_uart.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Physical UART/USART instances.
 * This represents the exhaustive superset available in the STM32F4 family.
 * Elements are named to match CMSIS definitions (USART vs UART).
 */
typedef enum
{
    CFN_HAL_UART_PORT_USART1,
    CFN_HAL_UART_PORT_USART2,
    CFN_HAL_UART_PORT_USART3,
    CFN_HAL_UART_PORT_UART4,
    CFN_HAL_UART_PORT_UART5,
    CFN_HAL_UART_PORT_USART6,
    CFN_HAL_UART_PORT_UART7,
    CFN_HAL_UART_PORT_UART8,
    CFN_HAL_UART_PORT_UART9,
    CFN_HAL_UART_PORT_UART10,
    CFN_HAL_UART_PORT_MAX
} cfn_hal_uart_port_t;

/* Functions prototypes ---------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_uart_construct(cfn_hal_uart_t *driver, const cfn_hal_uart_config_t *config, const cfn_hal_uart_phy_t *phy);

cfn_hal_error_code_t cfn_hal_uart_destruct(cfn_hal_uart_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_UART_STM32F4_H */
