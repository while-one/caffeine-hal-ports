/**
 * @file cfn_hal_stm32_error.h
 * @brief STM32-specific HAL status mapping to Caffeine-HAL error codes.
 */

#ifndef CAFFEINE_HAL_PORT_STM32_ERROR_H
#define CAFFEINE_HAL_PORT_STM32_ERROR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_types.h"
#include "stm32f4xx_hal.h"

/* Defines ----------------------------------------------------------*/

/**
 * @brief Number of elements in HAL_StatusTypeDef.
 */
#define CFN_HAL_STM32_ERROR_MAX 4

/* Private Data -----------------------------------------------------*/

/**
 * @brief Internal mapping array from STM32 HAL_StatusTypeDef to cfn_hal_error_code_t.
 * The array is indexed by the actual HAL_StatusTypeDef enum constants.
 */
static const cfn_hal_error_code_t stm32_error_map[CFN_HAL_STM32_ERROR_MAX] = {
    [HAL_OK] = CFN_HAL_ERROR_OK,
    [HAL_ERROR] = CFN_HAL_ERROR_FAIL,
    [HAL_BUSY] = CFN_HAL_ERROR_BUSY,
    [HAL_TIMEOUT] = CFN_HAL_ERROR_TIMING_TIMEOUT,
};

/* Functions --------------------------------------------------------*/

/**
 * @brief Safe wrapper to map STM32 HAL_StatusTypeDef to cfn_hal_error_code_t.
 * @param status STM32 status code (HAL_StatusTypeDef).
 * @return Corresponding cfn_hal_error_code_t.
 */
static inline cfn_hal_error_code_t cfn_hal_stm32_map_error(HAL_StatusTypeDef status)
{
    if ((uint32_t) status >= CFN_HAL_STM32_ERROR_MAX)
    {
        return CFN_HAL_ERROR_FAIL;
    }
    return stm32_error_map[status];
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_STM32_ERROR_H */
