/**
 * Copyright (c) 2026 Hisham Moussa Daou <https://www.whileone.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file cfn_hal_stm32_error.h
 * @brief STM32F4 HAL Error Definitions
 */

#ifndef CAFFEINE_HAL_PORT_STM32_ERROR_H
#define CAFFEINE_HAL_PORT_STM32_ERROR_H

#ifdef __cplusplus
extern "C" {
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
 * @brief Internal mapping array from STM32 HAL_StatusTypeDef to
 * cfn_hal_error_code_t. The array is indexed by the actual HAL_StatusTypeDef
 * enum constants.
 */
static const cfn_hal_error_code_t STM32_ERROR_MAP[CFN_HAL_STM32_ERROR_MAX] = {
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
static inline cfn_hal_error_code_t
cfn_hal_stm32_map_error(HAL_StatusTypeDef status) {
  if ((uint32_t)status >= CFN_HAL_STM32_ERROR_MAX) {
    return CFN_HAL_ERROR_FAIL;
  }
  return STM32_ERROR_MAP[status];
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_STM32_ERROR_H */
