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
 * @file cfn_hal_i2c_port.c
 * @brief STM32F4 I2C HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_i2c_port.h"
#include "cfn_hal_clock.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_i2c.h"
#include "cfn_hal_stm32_error.h"
#include "stm32f4xx_hal.h"

/* Private Prototypes ----------------------------------------------*/
/* Private Prototypes ----------------------------------------------*/
#ifdef HAL_I2C_MODULE_ENABLED

/* Private Data -----------------------------------------------------*/
static I2C_TypeDef *const PORT_INSTANCES[CFN_HAL_I2C_PORT_MAX] = {
#if defined(I2C1)
    [CFN_HAL_I2C_PORT_I2C1] = I2C1,
#endif
#if defined(I2C2)
    [CFN_HAL_I2C_PORT_I2C2] = I2C2,
#endif
#if defined(I2C3)
    [CFN_HAL_I2C_PORT_I2C3] = I2C3,
#endif
};

static const uint32_t PORT_MAP_PERIPHERAL_ID[CFN_HAL_I2C_PORT_MAX] = {
    [CFN_HAL_I2C_PORT_I2C1] = CFN_HAL_PORT_PERIPH_I2C1,
    [CFN_HAL_I2C_PORT_I2C2] = CFN_HAL_PORT_PERIPH_I2C2,
    [CFN_HAL_I2C_PORT_I2C3] = CFN_HAL_PORT_PERIPH_I2C3,
};

static I2C_HandleTypeDef port_hi2cs[CFN_HAL_I2C_PORT_MAX];
static cfn_hal_i2c_t *port_drivers[CFN_HAL_I2C_PORT_MAX];

/* Internal Helpers -------------------------------------------------*/

static uint32_t get_port_id_from_handle(I2C_HandleTypeDef *hi2c) {
  if ((hi2c < &port_hi2cs[0]) || (hi2c >= &port_hi2cs[CFN_HAL_I2C_PORT_MAX])) {
    return UINT32_MAX;
  }
  return (uint32_t)(hi2c - port_hi2cs);
}

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t low_level_init(cfn_hal_i2c_t *driver) {
  if (driver == NULL || driver->phy == NULL) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  struct cfn_hal_clock_s *clk = driver->base.clock_driver;
  if (clk == NULL) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  if (port_id >= CFN_HAL_I2C_PORT_MAX) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  /* 1. Enable Clock */
  cfn_hal_clock_enable_gate((cfn_hal_clock_t *)clk, driver->base.peripheral_id);

  /* 2. Initialize Pins */
  if (driver->phy->sda) {
    (void)cfn_hal_gpio_init(driver->phy->sda->port);
  }
  if (driver->phy->scl) {
    (void)cfn_hal_gpio_init(driver->phy->scl->port);
  }

  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base) {
  cfn_hal_i2c_t *driver = (cfn_hal_i2c_t *)base;
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  I2C_HandleTypeDef *hi2c = &port_hi2cs[port_id];

  cfn_hal_error_code_t error = low_level_init(driver);
  if (error != CFN_HAL_ERROR_OK) {
    return error;
  }

  hi2c->Instance = PORT_INSTANCES[port_id];

  switch (driver->config->speed) {
  case CFN_HAL_I2C_CONFIG_SPEED_100KHZ:
    hi2c->Init.ClockSpeed = 100000;
    break;
  case CFN_HAL_I2C_CONFIG_SPEED_400KHZ:
    hi2c->Init.ClockSpeed = 400000;
    break;
  default:
    hi2c->Init.ClockSpeed = 100000;
    break;
  }

  hi2c->Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c->Init.OwnAddress1 = 0;
  hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c->Init.OwnAddress2 = 0;
  hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  return cfn_hal_stm32_map_error(HAL_I2C_Init(hi2c));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base) {
  cfn_hal_i2c_t *driver = (cfn_hal_i2c_t *)base;
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  return cfn_hal_stm32_map_error(HAL_I2C_DeInit(&port_hi2cs[port_id]));
}

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base,
                                                 const void *config) {
  CFN_HAL_UNUSED(base);
  CFN_HAL_UNUSED(config);
  return port_base_init(base);
}

static cfn_hal_error_code_t port_base_event_enable(cfn_hal_driver_t *base,
                                                   uint32_t event_mask) {
  cfn_hal_i2c_t *driver = (cfn_hal_i2c_t *)base;
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  I2C_HandleTypeDef *hi2c = &port_hi2cs[port_id];

  if (event_mask &
      (CFN_HAL_I2C_EVENT_TX_COMPLETE | CFN_HAL_I2C_EVENT_RX_READY)) {
    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT);
    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_BUF);
  }

  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base,
                                                    uint32_t event_mask) {
  cfn_hal_i2c_t *driver = (cfn_hal_i2c_t *)base;
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  I2C_HandleTypeDef *hi2c = &port_hi2cs[port_id];

  if (event_mask &
      (CFN_HAL_I2C_EVENT_TX_COMPLETE | CFN_HAL_I2C_EVENT_RX_READY)) {
    __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT);
    __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_BUF);
  }

  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base,
                                                uint32_t *event_mask) {
  cfn_hal_i2c_t *driver = (cfn_hal_i2c_t *)base;
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  I2C_HandleTypeDef *hi2c = &port_hi2cs[port_id];
  uint32_t mask = 0;

  if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE)) {
    mask |= CFN_HAL_I2C_EVENT_TX_COMPLETE;
  }
  if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE)) {
    mask |= CFN_HAL_I2C_EVENT_RX_READY;
  }

  if (event_mask != NULL) {
    *event_mask = mask;
  }
  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base,
                                                   uint32_t error_mask) {
  cfn_hal_i2c_t *driver = (cfn_hal_i2c_t *)base;
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  I2C_HandleTypeDef *hi2c = &port_hi2cs[port_id];

  if (error_mask != 0) {
    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_ERR);
  }

  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base,
                                                    uint32_t error_mask) {
  cfn_hal_i2c_t *driver = (cfn_hal_i2c_t *)base;
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  I2C_HandleTypeDef *hi2c = &port_hi2cs[port_id];

  if (error_mask != 0) {
    __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_ERR);
  }

  return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base,
                                                uint32_t *error_mask) {
  cfn_hal_i2c_t *driver = (cfn_hal_i2c_t *)base;
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  I2C_HandleTypeDef *hi2c = &port_hi2cs[port_id];
  uint32_t mask = 0;

  if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF)) {
    mask |= CFN_HAL_I2C_ERROR_ACK;
  }
  if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BERR)) {
    mask |= CFN_HAL_I2C_ERROR_SMB;
  }
  if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ARLO)) {
    mask |= CFN_HAL_I2C_ERROR_ARBITRATION;
  }

  if (error_mask != NULL) {
    *error_mask = mask;
  }
  return CFN_HAL_ERROR_OK;
}

/* ST HAL Callback Overrides ----------------------------------------*/

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
  uint32_t port_id = get_port_id_from_handle(hi2c);
  if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL)) {
    cfn_hal_i2c_t *driver = port_drivers[port_id];
    if (driver->cb != NULL) {
      driver->cb(driver, CFN_HAL_I2C_EVENT_TX_COMPLETE, CFN_HAL_I2C_ERROR_NONE,
                 NULL, 0, driver->cb_user_arg);
    }
  }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
  uint32_t port_id = get_port_id_from_handle(hi2c);
  if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL)) {
    cfn_hal_i2c_t *driver = port_drivers[port_id];
    if (driver->cb != NULL) {
      driver->cb(driver, CFN_HAL_I2C_EVENT_RX_READY, CFN_HAL_I2C_ERROR_NONE,
                 NULL, 0, driver->cb_user_arg);
    }
  }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
  uint32_t port_id = get_port_id_from_handle(hi2c);
  if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL)) {
    cfn_hal_i2c_t *driver = port_drivers[port_id];
    if (driver->cb != NULL) {
      uint32_t error_mask = 0;
      (void)port_base_error_get(&driver->base, &error_mask);
      driver->cb(driver, CFN_HAL_I2C_EVENT_NONE, error_mask, NULL, 0,
                 driver->cb_user_arg);
    }
  }
}

/* Raw ISR Handlers -------------------------------------------------*/

#ifndef CFN_HAL_PORT_DISABLE_IRQ_I2C
void I2C1_ER_IRQHandler(void); // NOLINT(readability-identifier-naming)
void I2C1_EV_IRQHandler(void); // NOLINT(readability-identifier-naming)
void I2C2_ER_IRQHandler(void); // NOLINT(readability-identifier-naming)
void I2C2_EV_IRQHandler(void); // NOLINT(readability-identifier-naming)
void I2C3_ER_IRQHandler(void); // NOLINT(readability-identifier-naming)
void I2C3_EV_IRQHandler(void); // NOLINT(readability-identifier-naming)

#if defined(I2C1)
void I2C1_EV_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
  HAL_I2C_EV_IRQHandler(&port_hi2cs[CFN_HAL_I2C_PORT_I2C1]);
}
void I2C1_ER_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
  HAL_I2C_ER_IRQHandler(&port_hi2cs[CFN_HAL_I2C_PORT_I2C1]);
}
#endif

#if defined(I2C2)
void I2C2_EV_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
  HAL_I2C_EV_IRQHandler(&port_hi2cs[CFN_HAL_I2C_PORT_I2C2]);
}
void I2C2_ER_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
  HAL_I2C_ER_IRQHandler(&port_hi2cs[CFN_HAL_I2C_PORT_I2C2]);
}
#endif

#if defined(I2C3)
void I2C3_EV_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
  HAL_I2C_EV_IRQHandler(&port_hi2cs[CFN_HAL_I2C_PORT_I2C3]);
}
void I2C3_ER_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
  HAL_I2C_ER_IRQHandler(&port_hi2cs[CFN_HAL_I2C_PORT_I2C3]);
}
#endif

#endif /* CFN_HAL_PORT_DISABLE_IRQ_I2C */

/* I2C Specific Functions */

static cfn_hal_error_code_t
port_i2c_xfr_polling(cfn_hal_i2c_t *driver,
                     const cfn_hal_i2c_transaction_t *xfr, uint32_t timeout) {
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  HAL_StatusTypeDef status = HAL_OK;

  if (xfr->nbr_of_tx_bytes > 0) {
    status = HAL_I2C_Master_Transmit(
        &port_hi2cs[port_id], xfr->slave_address << 1,
        (uint8_t *)xfr->tx_payload, (uint16_t)xfr->nbr_of_tx_bytes, timeout);
    if (status != HAL_OK) {
      return cfn_hal_stm32_map_error(status);
    }
  }

  if (xfr->nbr_of_rx_bytes > 0) {
    status = HAL_I2C_Master_Receive(&port_hi2cs[port_id],
                                    xfr->slave_address << 1, xfr->rx_payload,
                                    (uint16_t)xfr->nbr_of_rx_bytes, timeout);
  }

  return cfn_hal_stm32_map_error(status);
}

static cfn_hal_error_code_t
port_i2c_mem_write(cfn_hal_i2c_t *driver,
                   const cfn_hal_i2c_mem_transaction_t *mem_xfr,
                   uint32_t timeout) {
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  uint32_t st_mem_size = (mem_xfr->mem_addr_size == 1) ? I2C_MEMADD_SIZE_8BIT
                                                       : I2C_MEMADD_SIZE_16BIT;
  return cfn_hal_stm32_map_error(HAL_I2C_Mem_Write(
      &port_hi2cs[port_id], mem_xfr->dev_addr << 1, mem_xfr->mem_addr,
      st_mem_size, mem_xfr->data, (uint16_t)mem_xfr->size, timeout));
}

static cfn_hal_error_code_t
port_i2c_mem_read(cfn_hal_i2c_t *driver,
                  const cfn_hal_i2c_mem_transaction_t *mem_xfr,
                  uint32_t timeout) {
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  uint32_t st_mem_size = (mem_xfr->mem_addr_size == 1) ? I2C_MEMADD_SIZE_8BIT
                                                       : I2C_MEMADD_SIZE_16BIT;
  return cfn_hal_stm32_map_error(HAL_I2C_Mem_Read(
      &port_hi2cs[port_id], mem_xfr->dev_addr << 1, mem_xfr->mem_addr,
      st_mem_size, mem_xfr->data, (uint16_t)mem_xfr->size, timeout));
}

static cfn_hal_error_code_t
port_i2c_xfr_irq(cfn_hal_i2c_t *driver, const cfn_hal_i2c_transaction_t *xfr) {
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  HAL_StatusTypeDef status = HAL_OK;

  if (xfr->nbr_of_tx_bytes > 0) {
    status = HAL_I2C_Master_Transmit_IT(
        &port_hi2cs[port_id], xfr->slave_address << 1,
        (uint8_t *)xfr->tx_payload, (uint16_t)xfr->nbr_of_tx_bytes);
  } else if (xfr->nbr_of_rx_bytes > 0) {
    status = HAL_I2C_Master_Receive_IT(&port_hi2cs[port_id],
                                       xfr->slave_address << 1, xfr->rx_payload,
                                       (uint16_t)xfr->nbr_of_rx_bytes);
  } else {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  return cfn_hal_stm32_map_error(status);
}

static cfn_hal_error_code_t port_i2c_xfr_irq_abort(cfn_hal_i2c_t *driver) {
  uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
  return cfn_hal_stm32_map_error(
      HAL_I2C_Master_Abort_IT(&port_hi2cs[port_id], 0));
}

/* API --------------------------------------------------------------*/
static const cfn_hal_i2c_api_t I2C_API = {
    .base =
        {
            .init = port_base_init,
            .deinit = port_base_deinit,
            .power_state_set = NULL,
            .config_set = port_base_config_set,
            .config_validate = NULL,
            .callback_register = NULL,
            .event_enable = port_base_event_enable,
            .event_disable = port_base_event_disable,
            .event_get = port_base_event_get,
            .error_enable = port_base_error_enable,
            .error_disable = port_base_error_disable,
            .error_get = port_base_error_get,
        },
    .xfr_irq = port_i2c_xfr_irq,
    .xfr_irq_abort = port_i2c_xfr_irq_abort,
    .xfr_polling = port_i2c_xfr_polling,
    .mem_read = port_i2c_mem_read,
    .mem_write = port_i2c_mem_write,
    .xfr_dma = NULL};

#endif /* HAL_I2C_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t cfn_hal_i2c_construct(cfn_hal_i2c_t *driver,
                                           const cfn_hal_i2c_config_t *config,
                                           const cfn_hal_i2c_phy_t *phy,
                                           struct cfn_hal_clock_s *clock,
                                           cfn_hal_i2c_callback_t callback,
                                           void *user_arg) {
#ifdef HAL_I2C_MODULE_ENABLED
  if ((driver == NULL) || (phy == NULL)) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  uint32_t port_id = (uint32_t)(uintptr_t)phy->instance;
  if (port_id >= CFN_HAL_I2C_PORT_MAX || PORT_INSTANCES[port_id] == NULL) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  uint32_t peripheral_id = PORT_MAP_PERIPHERAL_ID[port_id];
  cfn_hal_i2c_populate(driver, peripheral_id, clock, &I2C_API, phy, config,
                       callback, user_arg);

  port_hi2cs[port_id].Instance = PORT_INSTANCES[port_id];
  port_drivers[port_id] = driver;

  return CFN_HAL_ERROR_OK;
#else
  CFN_HAL_UNUSED(driver);
  CFN_HAL_UNUSED(config);
  CFN_HAL_UNUSED(phy);
  CFN_HAL_UNUSED(clock);
  CFN_HAL_UNUSED(callback);
  CFN_HAL_UNUSED(user_arg);
  return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}

cfn_hal_error_code_t cfn_hal_i2c_destruct(cfn_hal_i2c_t *driver) {
#ifdef HAL_I2C_MODULE_ENABLED
  if (driver == NULL) {
    return CFN_HAL_ERROR_BAD_PARAM;
  }

  if (driver->phy != NULL) {
    uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->instance;
    if (port_id < CFN_HAL_I2C_PORT_MAX) {
      port_drivers[port_id] = NULL;
    }
  }

  driver->config = NULL;
  driver->phy = NULL;
  return CFN_HAL_ERROR_OK;
#else
  CFN_HAL_UNUSED(driver);
  return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}
