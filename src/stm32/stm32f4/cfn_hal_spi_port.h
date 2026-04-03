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
 * @file cfn_hal_spi_port.h
 * @brief STM32F4 SPI HAL Port Header
 */

#ifndef CAFFEINE_HAL_PORT_SPI_H
#define CAFFEINE_HAL_PORT_SPI_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_spi.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief SPI port identifiers for STM32F4.
 */
typedef enum
{
    CFN_HAL_SPI_PORT_1,
    CFN_HAL_SPI_PORT_2,
    CFN_HAL_SPI_PORT_3,
    CFN_HAL_SPI_PORT_4,
    CFN_HAL_SPI_PORT_5,
    CFN_HAL_SPI_PORT_6,
    CFN_HAL_SPI_PORT_MAX
} cfn_hal_spi_port_t;

/* Functions prototypes ---------------------------------------------*/

/**
 * @brief Construct the spi driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the configuration structure.
 * @param phy Pointer to the physical mapping structure.
 * @param clock Pointer to the clock instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_spi_construct(cfn_hal_spi_t              *driver,
                                           const cfn_hal_spi_config_t *config,
                                           const cfn_hal_spi_phy_t    *phy,
                                           struct cfn_hal_clock_s     *clock,
                                           cfn_hal_spi_callback_t      callback,
                                           void                       *user_arg);

/**
 * @brief Destruct the spi driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_spi_destruct(cfn_hal_spi_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_SPI_H */
