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

/* VMT Implementations ----------------------------------------------*/

/* ... standard base stubs ... */

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base,
                                                uint32_t *event_mask) {
  CFN_HAL_UNUSED(base);
  if (event_mask) {
    *event_mask = 0;
  }
  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base,
                                                uint32_t *error_mask) {
  CFN_HAL_UNUSED(base);
  if (error_mask) {
    *error_mask = 0;
  }
  return CFN_HAL_ERROR_OK;
}

/* Clock Specific Functions */

static cfn_hal_error_code_t port_clock_suspend_tick(cfn_hal_clock_t *driver) {
  CFN_HAL_UNUSED(driver);
  HAL_SuspendTick();
  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_clock_resume_tick(cfn_hal_clock_t *driver) {
  CFN_HAL_UNUSED(driver);
  HAL_ResumeTick();
  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_clock_get_system_freq(cfn_hal_clock_t *driver,
                                                       uint32_t *freq_hz) {
  CFN_HAL_UNUSED(driver);
  if (!freq_hz) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  *freq_hz = HAL_RCC_GetSysClockFreq();
  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_clock_get_peripheral_freq(cfn_hal_clock_t *driver, uint32_t peripheral_id,
                               uint32_t *freq_hz) {
  CFN_HAL_UNUSED(driver);
  if (!freq_hz) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  cfn_hal_port_peripheral_id_t periph_id =
      (cfn_hal_port_peripheral_id_t)peripheral_id;

  switch (periph_id) {
  /* AHB1 Peripherals */
  /* AHB2 Peripherals */
  case CFN_HAL_PORT_PERIPH_GPIOA:
  case CFN_HAL_PORT_PERIPH_GPIOB:
  case CFN_HAL_PORT_PERIPH_GPIOC:
  case CFN_HAL_PORT_PERIPH_GPIOD:
  case CFN_HAL_PORT_PERIPH_GPIOE:
  case CFN_HAL_PORT_PERIPH_GPIOF:
  case CFN_HAL_PORT_PERIPH_GPIOG:
  case CFN_HAL_PORT_PERIPH_GPIOH:
  case CFN_HAL_PORT_PERIPH_GPIOI:
  case CFN_HAL_PORT_PERIPH_GPIOJ:
  case CFN_HAL_PORT_PERIPH_GPIOK:
  case CFN_HAL_PORT_PERIPH_ETH:
  case CFN_HAL_PORT_PERIPH_USB_OTG_HS:
  case CFN_HAL_PORT_PERIPH_USB_OTG_FS:
  case CFN_HAL_PORT_PERIPH_DMA1:
  case CFN_HAL_PORT_PERIPH_DMA2:
  case CFN_HAL_PORT_PERIPH_CRYP:
  case CFN_HAL_PORT_PERIPH_HASH:
  case CFN_HAL_PORT_PERIPH_RNG:
    *freq_hz = HAL_RCC_GetHCLKFreq();
    break;

  /* APB1 Peripherals */
  case CFN_HAL_PORT_PERIPH_USART2:
  case CFN_HAL_PORT_PERIPH_USART3:
  case CFN_HAL_PORT_PERIPH_UART4:
  case CFN_HAL_PORT_PERIPH_UART5:
  case CFN_HAL_PORT_PERIPH_UART7:
  case CFN_HAL_PORT_PERIPH_UART8:
  case CFN_HAL_PORT_PERIPH_SPI2:
  case CFN_HAL_PORT_PERIPH_SPI3:
  case CFN_HAL_PORT_PERIPH_I2C1:
  case CFN_HAL_PORT_PERIPH_I2C2:
  case CFN_HAL_PORT_PERIPH_I2C3:
  case CFN_HAL_PORT_PERIPH_DAC1:
  case CFN_HAL_PORT_PERIPH_CAN1:
  case CFN_HAL_PORT_PERIPH_CAN2:
  case CFN_HAL_PORT_PERIPH_CAN3:
    *freq_hz = HAL_RCC_GetPCLK1Freq();
    break;

  /* APB1 Timers (x2 multiplier if APB1 prescaler != 1) */
  case CFN_HAL_PORT_PERIPH_TIM2:
  case CFN_HAL_PORT_PERIPH_TIM3:
  case CFN_HAL_PORT_PERIPH_TIM4:
  case CFN_HAL_PORT_PERIPH_TIM5:
  case CFN_HAL_PORT_PERIPH_TIM6:
  case CFN_HAL_PORT_PERIPH_TIM7:
  case CFN_HAL_PORT_PERIPH_TIM12:
  case CFN_HAL_PORT_PERIPH_TIM13:
  case CFN_HAL_PORT_PERIPH_TIM14: {
    uint32_t pclk = HAL_RCC_GetPCLK1Freq();
    if ((RCC->CFGR & RCC_CFGR_PPRE1) == 0) {
      *freq_hz = pclk;
    } else {
      *freq_hz = pclk * 2;
    }
    break;
  }

  /* APB2 Peripherals */
  case CFN_HAL_PORT_PERIPH_USART1:
  case CFN_HAL_PORT_PERIPH_USART6:
  case CFN_HAL_PORT_PERIPH_UART9:
  case CFN_HAL_PORT_PERIPH_UART10:
  case CFN_HAL_PORT_PERIPH_SPI1:
  case CFN_HAL_PORT_PERIPH_SPI4:
  case CFN_HAL_PORT_PERIPH_SPI5:
  case CFN_HAL_PORT_PERIPH_SPI6:
  case CFN_HAL_PORT_PERIPH_ADC1:
  case CFN_HAL_PORT_PERIPH_ADC2:
  case CFN_HAL_PORT_PERIPH_ADC3:
  case CFN_HAL_PORT_PERIPH_COMP:
    *freq_hz = HAL_RCC_GetPCLK2Freq();
    break;

  /* APB2 Timers (x2 multiplier if APB2 prescaler != 1) */
  case CFN_HAL_PORT_PERIPH_TIM1:
  case CFN_HAL_PORT_PERIPH_TIM8:
  case CFN_HAL_PORT_PERIPH_TIM9:
  case CFN_HAL_PORT_PERIPH_TIM10:
  case CFN_HAL_PORT_PERIPH_TIM11: {
    uint32_t pclk = HAL_RCC_GetPCLK2Freq();
    if ((RCC->CFGR & RCC_CFGR_PPRE2) == 0) {
      *freq_hz = pclk;
    } else {
      *freq_hz = pclk * 2;
    }
    break;
  }

  default:
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_clock_enable_gate(cfn_hal_clock_t *driver,
                                                   uint32_t peripheral_id) {
  CFN_HAL_UNUSED(driver);
  cfn_hal_port_clock_enable_gate((cfn_hal_port_peripheral_id_t)peripheral_id);
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

void cfn_hal_port_clock_enable_gate(cfn_hal_port_peripheral_id_t periph_id) {
  switch (periph_id) {
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
#if defined(CAN3)
  case CFN_HAL_PORT_PERIPH_CAN3:
    __HAL_RCC_CAN3_CLK_ENABLE();
    break;
#endif
#if defined(ETH_MAC_BASE)
  case CFN_HAL_PORT_PERIPH_ETH:
    __HAL_RCC_ETHMAC_CLK_ENABLE();
    break;
#endif
#if defined(DMA1)
  case CFN_HAL_PORT_PERIPH_DMA1:
    __HAL_RCC_DMA1_CLK_ENABLE();
    break;
#endif
#if defined(DMA2)
  case CFN_HAL_PORT_PERIPH_DMA2:
    __HAL_RCC_DMA2_CLK_ENABLE();
    break;
#endif
#if defined(CRYP)
  case CFN_HAL_PORT_PERIPH_CRYP:
    __HAL_RCC_CRYP_CLK_ENABLE();
    break;
#endif
#if defined(HASH)
  case CFN_HAL_PORT_PERIPH_HASH:
    __HAL_RCC_HASH_CLK_ENABLE();
    break;
#endif
#if defined(RNG)
  case CFN_HAL_PORT_PERIPH_RNG:
    __HAL_RCC_RNG_CLK_ENABLE();
    break;
#endif
#if defined(COMP) || defined(COMP1) || defined(SYSCFG_COMP1_OFFSET)
  case CFN_HAL_PORT_PERIPH_COMP:
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    break;
#endif
#if defined(USB_OTG_FS)
  case CFN_HAL_PORT_PERIPH_USB_OTG_FS:
    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
    break;
#endif
#if defined(USB_OTG_HS)
  case CFN_HAL_PORT_PERIPH_USB_OTG_HS:
    __HAL_RCC_USB_OTG_HS_CLK_ENABLE();
    break;
#endif
#if defined(TIM1)
  case CFN_HAL_PORT_PERIPH_TIM1:
    __HAL_RCC_TIM1_CLK_ENABLE();
    break;
#endif
#if defined(TIM2)
  case CFN_HAL_PORT_PERIPH_TIM2:
    __HAL_RCC_TIM2_CLK_ENABLE();
    break;
#endif
#if defined(TIM3)
  case CFN_HAL_PORT_PERIPH_TIM3:
    __HAL_RCC_TIM3_CLK_ENABLE();
    break;
#endif
#if defined(TIM4)
  case CFN_HAL_PORT_PERIPH_TIM4:
    __HAL_RCC_TIM4_CLK_ENABLE();
    break;
#endif
#if defined(TIM5)
  case CFN_HAL_PORT_PERIPH_TIM5:
    __HAL_RCC_TIM5_CLK_ENABLE();
    break;
#endif
#if defined(TIM6)
  case CFN_HAL_PORT_PERIPH_TIM6:
    __HAL_RCC_TIM6_CLK_ENABLE();
    break;
#endif
#if defined(TIM7)
  case CFN_HAL_PORT_PERIPH_TIM7:
    __HAL_RCC_TIM7_CLK_ENABLE();
    break;
#endif
#if defined(TIM8)
  case CFN_HAL_PORT_PERIPH_TIM8:
    __HAL_RCC_TIM8_CLK_ENABLE();
    break;
#endif
#if defined(TIM9)
  case CFN_HAL_PORT_PERIPH_TIM9:
    __HAL_RCC_TIM9_CLK_ENABLE();
    break;
#endif
#if defined(TIM10)
  case CFN_HAL_PORT_PERIPH_TIM10:
    __HAL_RCC_TIM10_CLK_ENABLE();
    break;
#endif
#if defined(TIM11)
  case CFN_HAL_PORT_PERIPH_TIM11:
    __HAL_RCC_TIM11_CLK_ENABLE();
    break;
#endif
#if defined(TIM12)
  case CFN_HAL_PORT_PERIPH_TIM12:
    __HAL_RCC_TIM12_CLK_ENABLE();
    break;
#endif
#if defined(TIM13)
  case CFN_HAL_PORT_PERIPH_TIM13:
    __HAL_RCC_TIM13_CLK_ENABLE();
    break;
#endif
#if defined(TIM14)
  case CFN_HAL_PORT_PERIPH_TIM14:
    __HAL_RCC_TIM14_CLK_ENABLE();
    break;
#endif
  default:
    break;
  }
}

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t cfn_hal_clock_construct(
    cfn_hal_clock_t *driver, const cfn_hal_clock_config_t *config,
    const cfn_hal_clock_phy_t *phy, struct cfn_hal_clock_s *clock,
    cfn_hal_clock_callback_t callback, void *user_arg) {
  if (driver == NULL || phy == NULL) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  cfn_hal_clock_populate(driver, 0, clock, &CLOCK_API, phy, config, callback,
                         user_arg);

  return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_clock_destruct(cfn_hal_clock_t *driver) {
  if (driver == NULL) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  driver->config = NULL;
  driver->phy = NULL;

  return CFN_HAL_ERROR_OK;
}
