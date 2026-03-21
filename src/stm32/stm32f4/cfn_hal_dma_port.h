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
 * @file cfn_hal_dma_port.h
 * @brief STM32F4 DMA HAL Port Header
 */

#ifndef CAFFEINE_HAL_PORT_DMA_STM32F4_H
#define CAFFEINE_HAL_PORT_DMA_STM32F4_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_dma.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Physical DMA Stream instances.
 * STM32F4 has 2 DMA controllers with 8 streams each.
 */
typedef enum {
  CFN_HAL_DMA_PORT_DMA1_STREAM0,
  CFN_HAL_DMA_PORT_DMA1_STREAM1,
  CFN_HAL_DMA_PORT_DMA1_STREAM2,
  CFN_HAL_DMA_PORT_DMA1_STREAM3,
  CFN_HAL_DMA_PORT_DMA1_STREAM4,
  CFN_HAL_DMA_PORT_DMA1_STREAM5,
  CFN_HAL_DMA_PORT_DMA1_STREAM6,
  CFN_HAL_DMA_PORT_DMA1_STREAM7,
  CFN_HAL_DMA_PORT_DMA2_STREAM0,
  CFN_HAL_DMA_PORT_DMA2_STREAM1,
  CFN_HAL_DMA_PORT_DMA2_STREAM2,
  CFN_HAL_DMA_PORT_DMA2_STREAM3,
  CFN_HAL_DMA_PORT_DMA2_STREAM4,
  CFN_HAL_DMA_PORT_DMA2_STREAM5,
  CFN_HAL_DMA_PORT_DMA2_STREAM6,
  CFN_HAL_DMA_PORT_DMA2_STREAM7,
  CFN_HAL_DMA_PORT_MAX
} cfn_hal_dma_port_t;

/**
 * @brief STM32F4 DMA Channel mapping (Request Selection).
 */
typedef enum {
  CFN_HAL_DMA_CHANNEL_0 = 0,
  CFN_HAL_DMA_CHANNEL_1,
  CFN_HAL_DMA_CHANNEL_2,
  CFN_HAL_DMA_CHANNEL_3,
  CFN_HAL_DMA_CHANNEL_4,
  CFN_HAL_DMA_CHANNEL_5,
  CFN_HAL_DMA_CHANNEL_6,
  CFN_HAL_DMA_CHANNEL_7
} cfn_hal_dma_channel_t;

/* Functions prototypes ---------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_DMA_STM32F4_H */
