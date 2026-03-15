/**
 * @file cfn_hal_vendor_error_port.h
 * @brief Template for mapping vendor-specific status codes using an array.
 *
 * This header should be copied and adapted for each vendor port.
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
    [VENDOR_OK] = CFN_HAL_ERROR_OK,
    [VENDOR_ERROR] = CFN_HAL_ERROR_FAIL,
    [VENDOR_BUSY] = CFN_HAL_ERROR_BUSY,
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
