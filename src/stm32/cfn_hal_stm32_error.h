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

/* STM32 HAL Status Type definition - Provided here as placeholder if not included */
#ifndef HAL_OK
typedef enum
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;
#endif

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
    [HAL_TIMEOUT] = CFN_HAL_ERROR_TIMEOUT,
};

/* Functions --------------------------------------------------------*/

/**
 * @brief Safe wrapper to map STM32 HAL_StatusTypeDef to cfn_hal_error_code_t.
 * @param status STM32 status code (HAL_StatusTypeDef).
 * @return Corresponding cfn_hal_error_code_t.
 */
static inline cfn_hal_error_code_t cfn_hal_stm32_map_error(int status)
{
    if (status < 0 || status >= CFN_HAL_STM32_ERROR_MAX)
    {
        return CFN_HAL_ERROR_FAIL;
    }
    return stm32_error_map[status];
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_STM32_ERROR_H */
