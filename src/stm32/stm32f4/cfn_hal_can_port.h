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
 * @file cfn_hal_can_port.h
 * @brief STM32F4 CAN HAL Port Header
 */

#ifndef CAFFEINE_HAL_PORT_CAN_STM32F4_H
#define CAFFEINE_HAL_PORT_CAN_STM32F4_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_can.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Physical CAN instances.
 */
typedef enum
{
    CFN_HAL_CAN_PORT_CAN1,
    CFN_HAL_CAN_PORT_CAN2,
    CFN_HAL_CAN_PORT_CAN3,

    CFN_HAL_CAN_PORT_MAX
} cfn_hal_can_port_t;

/* Functions prototypes ---------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_can_construct(cfn_hal_can_t *driver, const cfn_hal_can_config_t *config, const cfn_hal_can_phy_t *phy);

cfn_hal_error_code_t cfn_hal_can_destruct(cfn_hal_can_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_CAN_STM32F4_H */
