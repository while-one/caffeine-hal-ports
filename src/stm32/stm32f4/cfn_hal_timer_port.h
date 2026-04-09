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
 * @file cfn_hal_timer_port.h
 * @brief STM32F4 TIMER HAL Port Header
 */

#ifndef CAFFEINE_HAL_PORT_TIMER_H
#define CAFFEINE_HAL_PORT_TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_timer.h"

/* Functions prototypes ---------------------------------------------*/
typedef enum
{
    CFN_HAL_TIMER_PORT_TIM1,
    CFN_HAL_TIMER_PORT_TIM2,
    CFN_HAL_TIMER_PORT_TIM3,
    CFN_HAL_TIMER_PORT_TIM4,
    CFN_HAL_TIMER_PORT_TIM5,
    CFN_HAL_TIMER_PORT_TIM6,
    CFN_HAL_TIMER_PORT_TIM7,
    CFN_HAL_TIMER_PORT_TIM8,
    CFN_HAL_TIMER_PORT_TIM9,
    CFN_HAL_TIMER_PORT_TIM10,
    CFN_HAL_TIMER_PORT_TIM11,
    CFN_HAL_TIMER_PORT_TIM12,
    CFN_HAL_TIMER_PORT_TIM13,
    CFN_HAL_TIMER_PORT_TIM14,
    CFN_HAL_TIMER_PORT_MAX
} cfn_hal_timer_port_t;
#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_TIMER_H */
