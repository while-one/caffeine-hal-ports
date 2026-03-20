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
 * @file cfn_hal_clock_port.c
 * @brief STM32F4 CLOCK HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_clock.h"
#include "cfn_hal_clock_port.h"

/* VMT Implementations ----------------------------------------------*/

/* ... standard base stubs ... */

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask)
{
    CFN_HAL_UNUSED(base);
    if (event_mask)
    {
        *event_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    CFN_HAL_UNUSED(base);
    if (error_mask)
    {
        *error_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

/* Clock Specific Functions */

static cfn_hal_error_code_t port_clock_suspend_tick(cfn_hal_clock_t *driver)
{
    CFN_HAL_UNUSED(driver);
    HAL_SuspendTick();
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_clock_resume_tick(cfn_hal_clock_t *driver)
{
    CFN_HAL_UNUSED(driver);
    HAL_ResumeTick();
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_clock_get_system_freq(cfn_hal_clock_t *driver, uint32_t *freq_hz)
{
    CFN_HAL_UNUSED(driver);
    if (!freq_hz)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *freq_hz = HAL_RCC_GetSysClockFreq();
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_clock_get_peripheral_freq(cfn_hal_clock_t *driver, uint32_t peripheral_id, uint32_t *freq_hz)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(peripheral_id);
    if (!freq_hz)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *freq_hz = HAL_RCC_GetHCLKFreq();
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_clock_enable_gate(cfn_hal_clock_t *driver, uint32_t peripheral_id)
{
    CFN_HAL_UNUSED(driver);
    cfn_hal_port_clock_enable_gate((cfn_hal_port_peripheral_id_t) peripheral_id);
    return CFN_HAL_ERROR_OK;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_clock_api_t CLOCK_API = {
    .base = {
        .init = NULL,
        .deinit = NULL,
        .power_state_set = NULL,
        .config_set = NULL,
        .callback_register = NULL,
        .event_enable = NULL,
        .event_disable = NULL,
        .event_get = port_base_event_get,
        .error_enable = NULL,
        .error_disable = NULL,
        .error_get = port_base_error_get,
    },
    .suspend_tick = port_clock_suspend_tick,
    .resume_tick = port_clock_resume_tick,
    .get_system_freq = port_clock_get_system_freq,
    .get_peripheral_freq = port_clock_get_peripheral_freq,
    .enable_gate = port_clock_enable_gate,
    .disable_gate = NULL
};

/* Internal Helper --------------------------------------------------*/

void cfn_hal_port_clock_enable_gate(cfn_hal_port_peripheral_id_t periph_id)
{
    switch (periph_id)
    {
#if defined(GPIOA)
        case CFN_HAL_PORT_PERIPH_GPIOA:
            __HAL_RCC_GPIOA_CLK_ENABLE();
            break;
#endif
#if defined(GPIOB)
        case CFN_HAL_PORT_PERIPH_GPIOB:
            __HAL_RCC_GPIOB_CLK_ENABLE();
            break;
#endif
#if defined(GPIOC)
        case CFN_HAL_PORT_PERIPH_GPIOC:
            __HAL_RCC_GPIOC_CLK_ENABLE();
            break;
#endif
#if defined(GPIOD)
        case CFN_HAL_PORT_PERIPH_GPIOD:
            __HAL_RCC_GPIOD_CLK_ENABLE();
            break;
#endif
#if defined(GPIOE)
        case CFN_HAL_PORT_PERIPH_GPIOE:
            __HAL_RCC_GPIOE_CLK_ENABLE();
            break;
#endif
#if defined(GPIOF)
        case CFN_HAL_PORT_PERIPH_GPIOF:
            __HAL_RCC_GPIOF_CLK_ENABLE();
            break;
#endif
#if defined(GPIOG)
        case CFN_HAL_PORT_PERIPH_GPIOG:
            __HAL_RCC_GPIOG_CLK_ENABLE();
            break;
#endif
#if defined(GPIOH)
        case CFN_HAL_PORT_PERIPH_GPIOH:
            __HAL_RCC_GPIOH_CLK_ENABLE();
            break;
#endif
#if defined(GPIOI)
        case CFN_HAL_PORT_PERIPH_GPIOI:
            __HAL_RCC_GPIOI_CLK_ENABLE();
            break;
#endif
#if defined(GPIOJ)
        case CFN_HAL_PORT_PERIPH_GPIOJ:
            __HAL_RCC_GPIOJ_CLK_ENABLE();
            break;
#endif
#if defined(GPIOK)
        case CFN_HAL_PORT_PERIPH_GPIOK:
            __HAL_RCC_GPIOK_CLK_ENABLE();
            break;
#endif
#if defined(USART1)
        case CFN_HAL_PORT_PERIPH_USART1:
            __HAL_RCC_USART1_CLK_ENABLE();
            break;
#endif
#if defined(USART2)
        case CFN_HAL_PORT_PERIPH_USART2:
            __HAL_RCC_USART2_CLK_ENABLE();
            break;
#endif
#if defined(USART3)
        case CFN_HAL_PORT_PERIPH_USART3:
            __HAL_RCC_USART3_CLK_ENABLE();
            break;
#endif
#if defined(UART4)
        case CFN_HAL_PORT_PERIPH_UART4:
            __HAL_RCC_UART4_CLK_ENABLE();
            break;
#endif
#if defined(UART5)
        case CFN_HAL_PORT_PERIPH_UART5:
            __HAL_RCC_UART5_CLK_ENABLE();
            break;
#endif
#if defined(USART6)
        case CFN_HAL_PORT_PERIPH_USART6:
            __HAL_RCC_USART6_CLK_ENABLE();
            break;
#endif
#if defined(UART7)
        case CFN_HAL_PORT_PERIPH_UART7:
            __HAL_RCC_UART7_CLK_ENABLE();
            break;
#endif
#if defined(UART8)
        case CFN_HAL_PORT_PERIPH_UART8:
            __HAL_RCC_UART8_CLK_ENABLE();
            break;
#endif
#if defined(UART9)
        case CFN_HAL_PORT_PERIPH_UART9:
            __HAL_RCC_UART9_CLK_ENABLE();
            break;
#endif
#if defined(UART10)
        case CFN_HAL_PORT_PERIPH_UART10:
            __HAL_RCC_UART10_CLK_ENABLE();
            break;
#endif
#if defined(SPI1)
        case CFN_HAL_PORT_PERIPH_SPI1:
            __HAL_RCC_SPI1_CLK_ENABLE();
            break;
#endif
#if defined(SPI2)
        case CFN_HAL_PORT_PERIPH_SPI2:
            __HAL_RCC_SPI2_CLK_ENABLE();
            break;
#endif
#if defined(SPI3)
        case CFN_HAL_PORT_PERIPH_SPI3:
            __HAL_RCC_SPI3_CLK_ENABLE();
            break;
#endif
#if defined(SPI4)
        case CFN_HAL_PORT_PERIPH_SPI4:
            __HAL_RCC_SPI4_CLK_ENABLE();
            break;
#endif
#if defined(SPI5)
        case CFN_HAL_PORT_PERIPH_SPI5:
            __HAL_RCC_SPI5_CLK_ENABLE();
            break;
#endif
#if defined(SPI6)
        case CFN_HAL_PORT_PERIPH_SPI6:
            __HAL_RCC_SPI6_CLK_ENABLE();
            break;
#endif
#if defined(I2C1)
        case CFN_HAL_PORT_PERIPH_I2C1:
            __HAL_RCC_I2C1_CLK_ENABLE();
            break;
#endif
#if defined(I2C2)
        case CFN_HAL_PORT_PERIPH_I2C2:
            __HAL_RCC_I2C2_CLK_ENABLE();
            break;
#endif
#if defined(I2C3)
        case CFN_HAL_PORT_PERIPH_I2C3:
            __HAL_RCC_I2C3_CLK_ENABLE();
            break;
#endif
#if defined(ADC1)
        case CFN_HAL_PORT_PERIPH_ADC1:
            __HAL_RCC_ADC1_CLK_ENABLE();
            break;
#endif
#if defined(ADC2)
        case CFN_HAL_PORT_PERIPH_ADC2:
            __HAL_RCC_ADC2_CLK_ENABLE();
            break;
#endif
#if defined(ADC3)
        case CFN_HAL_PORT_PERIPH_ADC3:
            __HAL_RCC_ADC3_CLK_ENABLE();
            break;
#endif
#if defined(DAC)
        case CFN_HAL_PORT_PERIPH_DAC1:
            __HAL_RCC_DAC_CLK_ENABLE();
            break;
#endif
#if defined(CAN1)
        case CFN_HAL_PORT_PERIPH_CAN1:
            __HAL_RCC_CAN1_CLK_ENABLE();
            break;
#endif
#if defined(CAN2)
        case CFN_HAL_PORT_PERIPH_CAN2:
            __HAL_RCC_CAN2_CLK_ENABLE();
            break;
#endif
#if defined(ETH_MAC_BASE)
        case CFN_HAL_PORT_PERIPH_ETH:
            __HAL_RCC_ETHMAC_CLK_ENABLE();
            break;
#endif
#if defined(TIM1)
        case CFN_HAL_PORT_PERIPH_TIM1:
            __HAL_RCC_TIM1_CLK_ENABLE();
            break;
#endif
        default:
            break;
    }
}

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_clock_construct(cfn_hal_clock_t *driver, const cfn_hal_clock_config_t *config, const cfn_hal_clock_phy_t *phy)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api         = &CLOCK_API;
    driver->base.type   = CFN_HAL_PERIPHERAL_TYPE_CLOCK;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config      = config;
    driver->phy         = phy;

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_clock_destruct(cfn_hal_clock_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api         = NULL;
    driver->base.type   = CFN_HAL_PERIPHERAL_TYPE_CLOCK;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config      = NULL;
    driver->phy         = NULL;

    return CFN_HAL_ERROR_OK;
}
