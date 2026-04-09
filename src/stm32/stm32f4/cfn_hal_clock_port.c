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
#include "cfn_hal_clock_port.h"

void cfn_hal_port_clock_enable_gate(cfn_hal_port_peripheral_id_t periph_id);
#include "cfn_hal_clock.h"
#include "stm32f4xx_hal.h"

/* Private Data -----------------------------------------------------*/

typedef enum
{
    BUS_HCLK,
    BUS_PCLK1,
    BUS_PCLK1_TIM,
    BUS_PCLK2,
    BUS_PCLK2_TIM,
    BUS_UNKNOWN
} bus_type_t;

static const bus_type_t PORT_PERIPH_BUS_MAP[CFN_HAL_PORT_PERIPH_MAX] = {
    [CFN_HAL_PORT_PERIPH_GPIOA] = BUS_HCLK,      [CFN_HAL_PORT_PERIPH_GPIOB] = BUS_HCLK,
    [CFN_HAL_PORT_PERIPH_GPIOC] = BUS_HCLK,      [CFN_HAL_PORT_PERIPH_GPIOD] = BUS_HCLK,
    [CFN_HAL_PORT_PERIPH_GPIOE] = BUS_HCLK,      [CFN_HAL_PORT_PERIPH_GPIOF] = BUS_HCLK,
    [CFN_HAL_PORT_PERIPH_GPIOG] = BUS_HCLK,      [CFN_HAL_PORT_PERIPH_GPIOH] = BUS_HCLK,
    [CFN_HAL_PORT_PERIPH_GPIOI] = BUS_HCLK,      [CFN_HAL_PORT_PERIPH_GPIOJ] = BUS_HCLK,
    [CFN_HAL_PORT_PERIPH_GPIOK] = BUS_HCLK,      [CFN_HAL_PORT_PERIPH_ETH] = BUS_HCLK,
    [CFN_HAL_PORT_PERIPH_USB_OTG_HS] = BUS_HCLK, [CFN_HAL_PORT_PERIPH_USB_OTG_FS] = BUS_HCLK,
    [CFN_HAL_PORT_PERIPH_DMA1] = BUS_HCLK,       [CFN_HAL_PORT_PERIPH_DMA2] = BUS_HCLK,
    [CFN_HAL_PORT_PERIPH_CRYP] = BUS_HCLK,       [CFN_HAL_PORT_PERIPH_HASH] = BUS_HCLK,
    [CFN_HAL_PORT_PERIPH_RNG] = BUS_HCLK,        [CFN_HAL_PORT_PERIPH_USART2] = BUS_PCLK1,
    [CFN_HAL_PORT_PERIPH_USART3] = BUS_PCLK1,    [CFN_HAL_PORT_PERIPH_UART4] = BUS_PCLK1,
    [CFN_HAL_PORT_PERIPH_UART5] = BUS_PCLK1,     [CFN_HAL_PORT_PERIPH_UART7] = BUS_PCLK1,
    [CFN_HAL_PORT_PERIPH_UART8] = BUS_PCLK1,     [CFN_HAL_PORT_PERIPH_SPI2] = BUS_PCLK1,
    [CFN_HAL_PORT_PERIPH_SPI3] = BUS_PCLK1,      [CFN_HAL_PORT_PERIPH_I2C1] = BUS_PCLK1,
    [CFN_HAL_PORT_PERIPH_I2C2] = BUS_PCLK1,      [CFN_HAL_PORT_PERIPH_I2C3] = BUS_PCLK1,
    [CFN_HAL_PORT_PERIPH_DAC1] = BUS_PCLK1,      [CFN_HAL_PORT_PERIPH_CAN1] = BUS_PCLK1,
    [CFN_HAL_PORT_PERIPH_CAN2] = BUS_PCLK1,      [CFN_HAL_PORT_PERIPH_CAN3] = BUS_PCLK1,
    [CFN_HAL_PORT_PERIPH_TIM2] = BUS_PCLK1_TIM,  [CFN_HAL_PORT_PERIPH_TIM3] = BUS_PCLK1_TIM,
    [CFN_HAL_PORT_PERIPH_TIM4] = BUS_PCLK1_TIM,  [CFN_HAL_PORT_PERIPH_TIM5] = BUS_PCLK1_TIM,
    [CFN_HAL_PORT_PERIPH_TIM6] = BUS_PCLK1_TIM,  [CFN_HAL_PORT_PERIPH_TIM7] = BUS_PCLK1_TIM,
    [CFN_HAL_PORT_PERIPH_TIM12] = BUS_PCLK1_TIM, [CFN_HAL_PORT_PERIPH_TIM13] = BUS_PCLK1_TIM,
    [CFN_HAL_PORT_PERIPH_TIM14] = BUS_PCLK1_TIM, [CFN_HAL_PORT_PERIPH_USART1] = BUS_PCLK2,
    [CFN_HAL_PORT_PERIPH_USART6] = BUS_PCLK2,    [CFN_HAL_PORT_PERIPH_UART9] = BUS_PCLK2,
    [CFN_HAL_PORT_PERIPH_UART10] = BUS_PCLK2,    [CFN_HAL_PORT_PERIPH_SPI1] = BUS_PCLK2,
    [CFN_HAL_PORT_PERIPH_SPI4] = BUS_PCLK2,      [CFN_HAL_PORT_PERIPH_SPI5] = BUS_PCLK2,
    [CFN_HAL_PORT_PERIPH_SPI6] = BUS_PCLK2,      [CFN_HAL_PORT_PERIPH_ADC1] = BUS_PCLK2,
    [CFN_HAL_PORT_PERIPH_ADC2] = BUS_PCLK2,      [CFN_HAL_PORT_PERIPH_ADC3] = BUS_PCLK2,
    [CFN_HAL_PORT_PERIPH_COMP] = BUS_PCLK2,      [CFN_HAL_PORT_PERIPH_TIM1] = BUS_PCLK2_TIM,
    [CFN_HAL_PORT_PERIPH_TIM8] = BUS_PCLK2_TIM,  [CFN_HAL_PORT_PERIPH_TIM9] = BUS_PCLK2_TIM,
    [CFN_HAL_PORT_PERIPH_TIM10] = BUS_PCLK2_TIM, [CFN_HAL_PORT_PERIPH_TIM11] = BUS_PCLK2_TIM,
};

typedef struct
{
    volatile uint32_t *enr_reg;
    uint32_t           enable_bit;
} clk_gate_t;

static const clk_gate_t CLK_GATE_MAP[CFN_HAL_PORT_PERIPH_MAX] = {
#if defined(GPIOA)
    [CFN_HAL_PORT_PERIPH_GPIOA] = { &RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN  },
#endif
#if defined(GPIOB)
    [CFN_HAL_PORT_PERIPH_GPIOB] = { &RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN  },
#endif
#if defined(GPIOC)
    [CFN_HAL_PORT_PERIPH_GPIOC] = { &RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN  },
#endif
#if defined(GPIOD)
    [CFN_HAL_PORT_PERIPH_GPIOD] = { &RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN  },
#endif
#if defined(GPIOE)
    [CFN_HAL_PORT_PERIPH_GPIOE] = { &RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN  },
#endif
#if defined(GPIOF)
    [CFN_HAL_PORT_PERIPH_GPIOF] = { &RCC->AHB1ENR, RCC_AHB1ENR_GPIOFEN  },
#endif
#if defined(GPIOG)
    [CFN_HAL_PORT_PERIPH_GPIOG] = { &RCC->AHB1ENR, RCC_AHB1ENR_GPIOGEN  },
#endif
#if defined(GPIOH)
    [CFN_HAL_PORT_PERIPH_GPIOH] = { &RCC->AHB1ENR, RCC_AHB1ENR_GPIOHEN  },
#endif
#if defined(GPIOI)
    [CFN_HAL_PORT_PERIPH_GPIOI] = { &RCC->AHB1ENR, RCC_AHB1ENR_GPIOIEN  },
#endif
#if defined(GPIOJ)
    [CFN_HAL_PORT_PERIPH_GPIOJ] = { &RCC->AHB1ENR, RCC_AHB1ENR_GPIOJEN  },
#endif
#if defined(GPIOK)
    [CFN_HAL_PORT_PERIPH_GPIOK] = { &RCC->AHB1ENR, RCC_AHB1ENR_GPIOKEN  },
#endif
#if defined(USART1)
    [CFN_HAL_PORT_PERIPH_USART1] = { &RCC->APB2ENR, RCC_APB2ENR_USART1EN },
#endif
#if defined(USART2)
    [CFN_HAL_PORT_PERIPH_USART2] = { &RCC->APB1ENR, RCC_APB1ENR_USART2EN },
#endif
#if defined(USART3)
    [CFN_HAL_PORT_PERIPH_USART3] = { &RCC->APB1ENR, RCC_APB1ENR_USART3EN },
#endif
#if defined(UART4)
    [CFN_HAL_PORT_PERIPH_UART4] = { &RCC->APB1ENR, RCC_APB1ENR_UART4EN  },
#endif
#if defined(UART5)
    [CFN_HAL_PORT_PERIPH_UART5] = { &RCC->APB1ENR, RCC_APB1ENR_UART5EN  },
#endif
#if defined(USART6)
    [CFN_HAL_PORT_PERIPH_USART6] = { &RCC->APB2ENR, RCC_APB2ENR_USART6EN },
#endif
#if defined(UART7)
    [CFN_HAL_PORT_PERIPH_UART7] = { &RCC->APB1ENR, RCC_APB1ENR_UART7EN  },
#endif
#if defined(UART8)
    [CFN_HAL_PORT_PERIPH_UART8] = { &RCC->APB1ENR, RCC_APB1ENR_UART8EN  },
#endif
#if defined(UART9)
    [CFN_HAL_PORT_PERIPH_UART9] = { &RCC->APB2ENR, RCC_APB2ENR_UART9EN  },
#endif
#if defined(UART10)
    [CFN_HAL_PORT_PERIPH_UART10] = { &RCC->APB2ENR, RCC_APB2ENR_UART10EN },
#endif
#if defined(SPI1)
    [CFN_HAL_PORT_PERIPH_SPI1] = { &RCC->APB2ENR, RCC_APB2ENR_SPI1EN   },
#endif
#if defined(SPI2)
    [CFN_HAL_PORT_PERIPH_SPI2] = { &RCC->APB1ENR, RCC_APB1ENR_SPI2EN   },
#endif
#if defined(SPI3)
    [CFN_HAL_PORT_PERIPH_SPI3] = { &RCC->APB1ENR, RCC_APB1ENR_SPI3EN   },
#endif
#if defined(SPI4)
    [CFN_HAL_PORT_PERIPH_SPI4] = { &RCC->APB2ENR, RCC_APB2ENR_SPI4EN   },
#endif
#if defined(SPI5)
    [CFN_HAL_PORT_PERIPH_SPI5] = { &RCC->APB2ENR, RCC_APB2ENR_SPI5EN   },
#endif
#if defined(SPI6)
    [CFN_HAL_PORT_PERIPH_SPI6] = { &RCC->APB2ENR, RCC_APB2ENR_SPI6EN   },
#endif
#if defined(I2C1)
    [CFN_HAL_PORT_PERIPH_I2C1] = { &RCC->APB1ENR, RCC_APB1ENR_I2C1EN   },
#endif
#if defined(I2C2)
    [CFN_HAL_PORT_PERIPH_I2C2] = { &RCC->APB1ENR, RCC_APB1ENR_I2C2EN   },
#endif
#if defined(I2C3)
    [CFN_HAL_PORT_PERIPH_I2C3] = { &RCC->APB1ENR, RCC_APB1ENR_I2C3EN   },
#endif
#if defined(ADC1)
    [CFN_HAL_PORT_PERIPH_ADC1] = { &RCC->APB2ENR, RCC_APB2ENR_ADC1EN   },
#endif
#if defined(ADC2)
    [CFN_HAL_PORT_PERIPH_ADC2] = { &RCC->APB2ENR, RCC_APB2ENR_ADC2EN   },
#endif
#if defined(ADC3)
    [CFN_HAL_PORT_PERIPH_ADC3] = { &RCC->APB2ENR, RCC_APB2ENR_ADC3EN   },
#endif
#if defined(DAC)
    [CFN_HAL_PORT_PERIPH_DAC1] = { &RCC->APB1ENR, RCC_APB1ENR_DACEN    },
#endif
#if defined(CAN1)
    [CFN_HAL_PORT_PERIPH_CAN1] = { &RCC->APB1ENR, RCC_APB1ENR_CAN1EN   },
#endif
#if defined(CAN2)
    [CFN_HAL_PORT_PERIPH_CAN2] = { &RCC->APB1ENR, RCC_APB1ENR_CAN2EN   },
#endif
#if defined(CAN3)
    [CFN_HAL_PORT_PERIPH_CAN3] = { &RCC->APB1ENR, RCC_APB1ENR_CAN3EN   },
#endif
#if defined(ETH_MAC_BASE)
    [CFN_HAL_PORT_PERIPH_ETH] = { &RCC->AHB1ENR, RCC_AHB1ENR_ETHMACEN },
#endif
#if defined(DMA1)
    [CFN_HAL_PORT_PERIPH_DMA1] = { &RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN   },
#endif
#if defined(DMA2)
    [CFN_HAL_PORT_PERIPH_DMA2] = { &RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN   },
#endif
#if defined(CRYP)
    [CFN_HAL_PORT_PERIPH_CRYP] = { &RCC->AHB2ENR, RCC_AHB2ENR_CRYPEN   },
#endif
#if defined(HASH)
    [CFN_HAL_PORT_PERIPH_HASH] = { &RCC->AHB2ENR, RCC_AHB2ENR_HASHEN   },
#endif
#if defined(RNG)
    [CFN_HAL_PORT_PERIPH_RNG] = { &RCC->AHB2ENR, RCC_AHB2ENR_RNGEN    },
#endif
#if defined(COMP) || defined(COMP1) || defined(SYSCFG_COMP1_OFFSET)
    [CFN_HAL_PORT_PERIPH_COMP] = { &RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN },
#endif
#if defined(USB_OTG_FS)
    [CFN_HAL_PORT_PERIPH_USB_OTG_FS] = { &RCC->AHB2ENR, RCC_AHB2ENR_OTGFSEN  },
#endif
#if defined(USB_OTG_HS)
    [CFN_HAL_PORT_PERIPH_USB_OTG_HS] = { &RCC->AHB1ENR, RCC_AHB1ENR_OTGHSEN  },
#endif
#if defined(TIM1)
    [CFN_HAL_PORT_PERIPH_TIM1] = { &RCC->APB2ENR, RCC_APB2ENR_TIM1EN   },
#endif
#if defined(TIM2)
    [CFN_HAL_PORT_PERIPH_TIM2] = { &RCC->APB1ENR, RCC_APB1ENR_TIM2EN   },
#endif
#if defined(TIM3)
    [CFN_HAL_PORT_PERIPH_TIM3] = { &RCC->APB1ENR, RCC_APB1ENR_TIM3EN   },
#endif
#if defined(TIM4)
    [CFN_HAL_PORT_PERIPH_TIM4] = { &RCC->APB1ENR, RCC_APB1ENR_TIM4EN   },
#endif
#if defined(TIM5)
    [CFN_HAL_PORT_PERIPH_TIM5] = { &RCC->APB1ENR, RCC_APB1ENR_TIM5EN   },
#endif
#if defined(TIM6)
    [CFN_HAL_PORT_PERIPH_TIM6] = { &RCC->APB1ENR, RCC_APB1ENR_TIM6EN   },
#endif
#if defined(TIM7)
    [CFN_HAL_PORT_PERIPH_TIM7] = { &RCC->APB1ENR, RCC_APB1ENR_TIM7EN   },
#endif
#if defined(TIM8)
    [CFN_HAL_PORT_PERIPH_TIM8] = { &RCC->APB2ENR, RCC_APB2ENR_TIM8EN   },
#endif
#if defined(TIM9)
    [CFN_HAL_PORT_PERIPH_TIM9] = { &RCC->APB2ENR, RCC_APB2ENR_TIM9EN   },
#endif
#if defined(TIM10)
    [CFN_HAL_PORT_PERIPH_TIM10] = { &RCC->APB2ENR, RCC_APB2ENR_TIM10EN  },
#endif
#if defined(TIM11)
    [CFN_HAL_PORT_PERIPH_TIM11] = { &RCC->APB2ENR, RCC_APB2ENR_TIM11EN  },
#endif
#if defined(TIM12)
    [CFN_HAL_PORT_PERIPH_TIM12] = { &RCC->APB1ENR, RCC_APB1ENR_TIM12EN  },
#endif
#if defined(TIM13)
    [CFN_HAL_PORT_PERIPH_TIM13] = { &RCC->APB1ENR, RCC_APB1ENR_TIM13EN  },
#endif
#if defined(TIM14)
    [CFN_HAL_PORT_PERIPH_TIM14] = { &RCC->APB1ENR, RCC_APB1ENR_TIM14EN  },
#endif
};

/* Internal Helpers -------------------------------------------------*/

/**
 * @brief Calculates the frequency for a timer bus.
 * @details In STM32F4, if the APB prescaler is not 1, the timer frequency is 2x the bus frequency.
 */
static inline uint32_t calculate_timer_freq(uint32_t pclk, uint32_t ppre_mask)
{
    return ((RCC->CFGR & ppre_mask) == 0) ? pclk : (pclk * 2);
}

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
    if (!freq_hz || peripheral_id >= CFN_HAL_PORT_PERIPH_MAX)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    bus_type_t bus = PORT_PERIPH_BUS_MAP[peripheral_id];

    switch (bus)
    {
        case BUS_HCLK:
            *freq_hz = HAL_RCC_GetHCLKFreq();
            break;
        case BUS_PCLK1:
            *freq_hz = HAL_RCC_GetPCLK1Freq();
            break;
        case BUS_PCLK2:
            *freq_hz = HAL_RCC_GetPCLK2Freq();
            break;
        case BUS_PCLK1_TIM:
            *freq_hz = calculate_timer_freq(HAL_RCC_GetPCLK1Freq(), RCC_CFGR_PPRE1);
            break;
        case BUS_PCLK2_TIM:
            *freq_hz = calculate_timer_freq(HAL_RCC_GetPCLK2Freq(), RCC_CFGR_PPRE2);
            break;
        default:
            return CFN_HAL_ERROR_BAD_PARAM;
    }

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
    .base =
        {
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
    .disable_gate = NULL};

/* Internal Helper --------------------------------------------------*/
void cfn_hal_port_clock_enable_gate(cfn_hal_port_peripheral_id_t periph_id)
{
    if (periph_id < CFN_HAL_PORT_PERIPH_MAX)
    {
        const clk_gate_t *gate = &CLK_GATE_MAP[periph_id];

        if (gate->enr_reg != NULL)
        {
            uint32_t tmpreg;

            *gate->enr_reg |= gate->enable_bit;

            /* Delay after an RCC peripheral clock enabling */
            tmpreg = *gate->enr_reg & gate->enable_bit;
            CFN_HAL_UNUSED(tmpreg);
        }
    }
}
/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t cfn_hal_clock_construct(cfn_hal_clock_t              *driver,
                                             const cfn_hal_clock_config_t *config,
                                             const cfn_hal_clock_phy_t    *phy,
                                             struct cfn_hal_clock_s       *clock,
                                             void                         *dependency,
                                             cfn_hal_clock_callback_t      callback,
                                             void                         *user_arg)
{
    if (driver == NULL || phy == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    cfn_hal_clock_populate(driver, 0, clock, dependency, &CLOCK_API, phy, config, callback, user_arg);

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_clock_destruct(cfn_hal_clock_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    cfn_hal_clock_populate(driver, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    return CFN_HAL_ERROR_OK;
}
