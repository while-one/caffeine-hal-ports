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
 * @file cfn_hal_vendor_error_port.h
 * @brief Generic Vendor HAL Error Port Template Header
 */

#ifndef CAFFEINE_HAL_PORT_VENDOR_ERROR_H
#define CAFFEINE_HAL_PORT_VENDOR_ERROR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_types.h"

/* Defines ----------------------------------------------------------*/

/* Placeholder enums for documentation purposes */
#ifndef VENDOR_OK
#define VENDOR_OK      0
#define VENDOR_ERROR   1
#define VENDOR_BUSY    2
#define VENDOR_TIMEOUT 3
#define VENDOR_MAX     4
#endif

/* Private Data -----------------------------------------------------*/

/**
 * @brief Internal mapping array from vendor status to cfn_hal_error_code_t.
 * The array is indexed by the actual vendor enum constants.
 */
static const cfn_hal_error_code_t vendor_error_map[VENDOR_MAX] = {
    [VENDOR_OK]      = CFN_HAL_ERROR_OK,
    [VENDOR_ERROR]   = CFN_HAL_ERROR_FAIL,
    [VENDOR_BUSY]    = CFN_HAL_ERROR_BUSY,
    [VENDOR_TIMEOUT] = CFN_HAL_ERROR_TIMEOUT,
};

/* Functions --------------------------------------------------------*/

/**
 * @brief Safe wrapper to map vendor status codes with bounds checking.
 * @param status Vendor-specific status code.
 * @return Corresponding cfn_hal_error_code_t.
 */
static inline cfn_hal_error_code_t cfn_hal_vendor_map_error(int status)
{
    if (status < 0 || status >= VENDOR_MAX)
    {
        return CFN_HAL_ERROR_FAIL;
    }
    return vendor_error_map[status];
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_VENDOR_ERROR_H */
