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
 * @file cfn_hal_clock_port.h
 * @brief STM32F4 CLOCK HAL Port Header
 */

#ifndef CAFFEINE_HAL_PORT_CLOCK_STM32F4_H
#define CAFFEINE_HAL_PORT_CLOCK_STM32F4_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_clock.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Peripheral IDs for clock gating.
 * This represents the exhaustive superset available in the STM32F4 family.
 */
typedef enum
{
    CFN_HAL_PORT_PERIPH_GPIOA,
    CFN_HAL_PORT_PERIPH_GPIOB,
    CFN_HAL_PORT_PERIPH_GPIOC,
    CFN_HAL_PORT_PERIPH_GPIOD,
    CFN_HAL_PORT_PERIPH_GPIOE,
    CFN_HAL_PORT_PERIPH_GPIOF,
    CFN_HAL_PORT_PERIPH_GPIOG,
    CFN_HAL_PORT_PERIPH_GPIOH,
    CFN_HAL_PORT_PERIPH_GPIOI,
    CFN_HAL_PORT_PERIPH_GPIOJ,
    CFN_HAL_PORT_PERIPH_GPIOK,
    CFN_HAL_PORT_PERIPH_USART1,
    CFN_HAL_PORT_PERIPH_USART2,
    CFN_HAL_PORT_PERIPH_USART3,
    CFN_HAL_PORT_PERIPH_UART4,
    CFN_HAL_PORT_PERIPH_UART5,
    CFN_HAL_PORT_PERIPH_USART6,
    CFN_HAL_PORT_PERIPH_UART7,
    CFN_HAL_PORT_PERIPH_UART8,
    CFN_HAL_PORT_PERIPH_UART9,
    CFN_HAL_PORT_PERIPH_UART10,
    CFN_HAL_PORT_PERIPH_SPI1,
    CFN_HAL_PORT_PERIPH_SPI2,
    CFN_HAL_PORT_PERIPH_SPI3,
    CFN_HAL_PORT_PERIPH_SPI4,
    CFN_HAL_PORT_PERIPH_SPI5,
    CFN_HAL_PORT_PERIPH_SPI6,
    CFN_HAL_PORT_PERIPH_I2C1,
    CFN_HAL_PORT_PERIPH_I2C2,
    CFN_HAL_PORT_PERIPH_I2C3,
    CFN_HAL_PORT_PERIPH_ADC1,
    CFN_HAL_PORT_PERIPH_ADC2,
    CFN_HAL_PORT_PERIPH_ADC3,
    CFN_HAL_PORT_PERIPH_DAC1,
    CFN_HAL_PORT_PERIPH_CAN1,
    CFN_HAL_PORT_PERIPH_CAN2,
    CFN_HAL_PORT_PERIPH_CAN3,
    CFN_HAL_PORT_PERIPH_ETH,
    CFN_HAL_PORT_PERIPH_USB_OTG_FS,
    CFN_HAL_PORT_PERIPH_USB_OTG_HS,
    CFN_HAL_PORT_PERIPH_DMA1,
    CFN_HAL_PORT_PERIPH_DMA2,
    CFN_HAL_PORT_PERIPH_CRYP,
    CFN_HAL_PORT_PERIPH_HASH,
    CFN_HAL_PORT_PERIPH_RNG,
    CFN_HAL_PORT_PERIPH_COMP,
    CFN_HAL_PORT_PERIPH_TIM1,
    CFN_HAL_PORT_PERIPH_TIM2,
    CFN_HAL_PORT_PERIPH_TIM3,
    CFN_HAL_PORT_PERIPH_TIM4,
    CFN_HAL_PORT_PERIPH_TIM5,
    CFN_HAL_PORT_PERIPH_TIM6,
    CFN_HAL_PORT_PERIPH_TIM7,
    CFN_HAL_PORT_PERIPH_TIM8,
    CFN_HAL_PORT_PERIPH_TIM9,
    CFN_HAL_PORT_PERIPH_TIM10,
    CFN_HAL_PORT_PERIPH_TIM11,
    CFN_HAL_PORT_PERIPH_TIM12,
    CFN_HAL_PORT_PERIPH_TIM13,
    CFN_HAL_PORT_PERIPH_TIM14,
    CFN_HAL_PORT_PERIPH_SDIO,
    CFN_HAL_PORT_PERIPH_MAX
} cfn_hal_port_peripheral_id_t;

/* Functions prototypes ---------------------------------------------*/

/**
 * @brief Internal helper to enable peripheral clocks.
 */

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_CLOCK_STM32F4_H */
