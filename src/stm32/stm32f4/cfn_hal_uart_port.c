/**
 * @file cfn_hal_uart_port.c
 * @brief STM32F4 UART HAL Port Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_uart.h"
#include "cfn_hal_uart_port.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_stm32_error.h"

/* Private Data -----------------------------------------------------*/

static USART_TypeDef *const PORT_INSTANCES[CFN_HAL_UART_PORT_MAX] = {
#if defined(USART1)
    [CFN_HAL_UART_PORT_USART1] = USART1,
#endif
#if defined(USART2)
    [CFN_HAL_UART_PORT_USART2] = USART2,
#endif
#if defined(USART3)
    [CFN_HAL_UART_PORT_USART3] = USART3,
#endif
#if defined(UART4)
    [CFN_HAL_UART_PORT_UART4] = UART4,
#endif
#if defined(UART5)
    [CFN_HAL_UART_PORT_UART5] = UART5,
#endif
#if defined(USART6)
    [CFN_HAL_UART_PORT_USART6] = USART6,
#endif
};

static UART_HandleTypeDef port_huarts[CFN_HAL_UART_PORT_MAX];
static cfn_hal_uart_t    *port_drivers[CFN_HAL_UART_PORT_MAX];

/* Internal Helpers -------------------------------------------------*/

static int32_t get_port_id_from_handle(UART_HandleTypeDef *huart)
{
    for (uint32_t i = 0; i < CFN_HAL_UART_PORT_MAX; i++)
    {
        if (&port_huarts[i] == huart)
        {
            return (int32_t) i;
        }
    }
    return -1;
}

/* VMT Implementations ----------------------------------------------*/

static void low_level_init(cfn_hal_uart_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    /* 1. Enable Clock */
    cfn_hal_port_clock_enable_gate((cfn_hal_port_peripheral_id_t) (CFN_HAL_PORT_PERIPH_USART1 + port_id));

    /* 2. Initialize Pins */
    if (driver->phy->tx)
    {
        (void) cfn_hal_base_init((cfn_hal_driver_t *) driver->phy->tx->port, CFN_HAL_PERIPHERAL_TYPE_GPIO);
    }
    if (driver->phy->rx)
    {
        (void) cfn_hal_base_init((cfn_hal_driver_t *) driver->phy->rx->port, CFN_HAL_PERIPHERAL_TYPE_GPIO);
    }
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_uart_t     *driver = (cfn_hal_uart_t *) base;
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    UART_HandleTypeDef *huart = &port_huarts[port_id];

    low_level_init(driver);

    huart->Instance = PORT_INSTANCES[port_id];
    huart->Init.BaudRate = driver->config->baudrate;
    huart->Init.WordLength =
        (driver->config->data_len == CFN_HAL_UART_CONFIG_DATA_LEN_9) ? UART_WORDLENGTH_9B : UART_WORDLENGTH_8B;
    huart->Init.StopBits =
        (driver->config->stop_bits == CFN_HAL_UART_CONFIG_STOP_TWO_BITS) ? UART_STOPBITS_2 : UART_STOPBITS_1;
    huart->Init.Parity = (driver->config->parity == CFN_HAL_UART_CONFIG_PARITY_NONE)   ? UART_PARITY_NONE
                         : (driver->config->parity == CFN_HAL_UART_CONFIG_PARITY_EVEN) ? UART_PARITY_EVEN
                                                                                       : UART_PARITY_ODD;
    huart->Init.Mode = UART_MODE_TX_RX;
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;

    return cfn_hal_stm32_map_error(HAL_UART_Init(huart));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_uart_t *driver = (cfn_hal_uart_t *) base;
    uint32_t        port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_UART_DeInit(&port_huarts[port_id]));
}

/* ... base stubs ... */
static cfn_hal_error_code_t port_base_power_state_set(cfn_hal_driver_t *base, cfn_hal_power_state_t state)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(state);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(config);
    return port_base_init(base);
}
static cfn_hal_error_code_t
port_base_callback_register(cfn_hal_driver_t *base, cfn_hal_callback_t callback, void *user_arg)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(callback);
    CFN_HAL_UNUSED(user_arg);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_enable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    cfn_hal_uart_t     *driver = (cfn_hal_uart_t *) base;
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    UART_HandleTypeDef *huart = &port_huarts[port_id];

    if (event_mask & CFN_HAL_UART_EVENT_RX_READY)
    {
        __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
    }
    if (event_mask & CFN_HAL_UART_EVENT_TX_COMPLETE)
    {
        __HAL_UART_ENABLE_IT(huart, UART_IT_TC);
    }
    if (event_mask & CFN_HAL_UART_EVENT_BUS_IDLE)
    {
        __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    cfn_hal_uart_t     *driver = (cfn_hal_uart_t *) base;
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    UART_HandleTypeDef *huart = &port_huarts[port_id];

    if (event_mask & CFN_HAL_UART_EVENT_RX_READY)
    {
        __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
    }
    if (event_mask & CFN_HAL_UART_EVENT_TX_COMPLETE)
    {
        __HAL_UART_DISABLE_IT(huart, UART_IT_TC);
    }
    if (event_mask & CFN_HAL_UART_EVENT_BUS_IDLE)
    {
        __HAL_UART_DISABLE_IT(huart, UART_IT_IDLE);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask)
{
    cfn_hal_uart_t     *driver = (cfn_hal_uart_t *) base;
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    UART_HandleTypeDef *huart = &port_huarts[port_id];
    uint32_t            mask = 0;

    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE))
    {
        mask |= CFN_HAL_UART_EVENT_RX_READY;
    }
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_TC))
    {
        mask |= CFN_HAL_UART_EVENT_TX_COMPLETE;
    }
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))
    {
        mask |= CFN_HAL_UART_EVENT_BUS_IDLE;
    }

    if (event_mask != NULL)
    {
        *event_mask = mask;
    }
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_uart_t     *driver = (cfn_hal_uart_t *) base;
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    UART_HandleTypeDef *huart = &port_huarts[port_id];

    if (error_mask & CFN_HAL_UART_ERROR_PARITY)
    {
        __HAL_UART_ENABLE_IT(huart, UART_IT_PE);
    }
    if (error_mask & (CFN_HAL_UART_ERROR_FRAMING | CFN_HAL_UART_ERROR_OVERRUN))
    {
        __HAL_UART_ENABLE_IT(huart, UART_IT_ERR);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    cfn_hal_uart_t     *driver = (cfn_hal_uart_t *) base;
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    UART_HandleTypeDef *huart = &port_huarts[port_id];

    if (error_mask & CFN_HAL_UART_ERROR_PARITY)
    {
        __HAL_UART_DISABLE_IT(huart, UART_IT_PE);
    }
    if (error_mask & (CFN_HAL_UART_ERROR_FRAMING | CFN_HAL_UART_ERROR_OVERRUN))
    {
        __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    cfn_hal_uart_t     *driver = (cfn_hal_uart_t *) base;
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    UART_HandleTypeDef *huart = &port_huarts[port_id];
    uint32_t            mask = 0;

    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_PE))
    {
        mask |= CFN_HAL_UART_ERROR_PARITY;
    }
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_FE))
    {
        mask |= CFN_HAL_UART_ERROR_FRAMING;
    }
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE))
    {
        mask |= CFN_HAL_UART_ERROR_OVERRUN;
    }

    if (error_mask != NULL)
    {
        *error_mask = mask;
    }
    return CFN_HAL_ERROR_OK;
}

/* ST HAL Callback Overrides ----------------------------------------*/

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    int32_t port_id = get_port_id_from_handle(huart);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_uart_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_UART_EVENT_TX_COMPLETE, CFN_HAL_UART_ERROR_NONE, NULL, 0, driver->cb_user_arg);
        }
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    int32_t port_id = get_port_id_from_handle(huart);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_uart_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_UART_EVENT_RX_READY, CFN_HAL_UART_ERROR_NONE, NULL, 0, driver->cb_user_arg);
        }
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    int32_t port_id = get_port_id_from_handle(huart);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_uart_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            uint32_t error_mask = 0;
            (void) port_base_error_get(&driver->base, &error_mask);
            driver->cb(driver, CFN_HAL_UART_EVENT_NONE, error_mask, NULL, 0, driver->cb_user_arg);
        }
    }
}

/* Raw ISR Handlers -------------------------------------------------*/

#if defined(USART1)
void USART1_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_UART_IRQHandler(&port_huarts[CFN_HAL_UART_PORT_USART1]);
}
#endif

#if defined(USART2)
void USART2_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_UART_IRQHandler(&port_huarts[CFN_HAL_UART_PORT_USART2]);
}
#endif

#if defined(USART3)
void USART3_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_UART_IRQHandler(&port_huarts[CFN_HAL_UART_PORT_USART3]);
}
#endif

#if defined(UART4)
void UART4_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_UART_IRQHandler(&port_huarts[CFN_HAL_UART_PORT_UART4]);
}
#endif

#if defined(UART5)
void UART5_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_UART_IRQHandler(&port_huarts[CFN_HAL_UART_PORT_UART5]);
}
#endif

#if defined(USART6)
void USART6_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_UART_IRQHandler(&port_huarts[CFN_HAL_UART_PORT_USART6]);
}
#endif

/* UART Specific Functions */

static cfn_hal_error_code_t
port_uart_tx_polling(cfn_hal_uart_t *driver, const uint8_t *data, size_t length, uint32_t timeout)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(
        HAL_UART_Transmit(&port_huarts[port_id], (uint8_t *) data, (uint16_t) length, timeout));
}

static cfn_hal_error_code_t
port_uart_rx_polling(cfn_hal_uart_t *driver, uint8_t *buffer, size_t length, uint32_t timeout)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_UART_Receive(&port_huarts[port_id], buffer, (uint16_t) length, timeout));
}

static cfn_hal_error_code_t
port_uart_rx_to_idle(cfn_hal_uart_t *driver, uint8_t *data, size_t max_bytes, size_t *received_bytes, uint32_t timeout)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(max_bytes);
    CFN_HAL_UNUSED(received_bytes);
    CFN_HAL_UNUSED(timeout);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

static cfn_hal_error_code_t port_uart_tx_irq(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(
        HAL_UART_Transmit_IT(&port_huarts[port_id], (uint8_t *) data, (uint16_t) nbr_of_bytes));
}

static cfn_hal_error_code_t port_uart_tx_irq_abort(cfn_hal_uart_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_UART_AbortTransmit_IT(&port_huarts[port_id]));
}

static cfn_hal_error_code_t port_uart_rx_irq(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_UART_Receive_IT(&port_huarts[port_id], data, (uint16_t) nbr_of_bytes));
}

static cfn_hal_error_code_t port_uart_rx_irq_abort(cfn_hal_uart_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_UART_AbortReceive_IT(&port_huarts[port_id]));
}
static cfn_hal_error_code_t port_uart_tx_dma(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(nbr_of_bytes);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}
static cfn_hal_error_code_t port_uart_rx_dma(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(data);
    CFN_HAL_UNUSED(nbr_of_bytes);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_uart_api_t UART_API = {
    .base = {
        .init = port_base_init,
        .deinit = port_base_deinit,
        .power_state_set = port_base_power_state_set,
        .config_set = port_base_config_set,
        .callback_register = port_base_callback_register,
        .event_enable = port_base_event_enable,
        .event_disable = port_base_event_disable,
        .event_get = port_base_event_get,
        .error_enable = port_base_error_enable,
        .error_disable = port_base_error_disable,
        .error_get = port_base_error_get,
    },
    .tx_irq = port_uart_tx_irq,
    .tx_irq_abort = port_uart_tx_irq_abort,
    .rx_irq = port_uart_rx_irq,
    .rx_irq_abort = port_uart_rx_irq_abort,
    .tx_polling = port_uart_tx_polling,
    .rx_polling = port_uart_rx_polling,
    .rx_to_idle = port_uart_rx_to_idle,
    .tx_dma = port_uart_tx_dma,
    .rx_dma = port_uart_rx_dma
};

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_uart_construct(cfn_hal_uart_t *driver, const cfn_hal_uart_config_t *config, const cfn_hal_uart_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_UART_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &UART_API;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_UART;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;

    port_huarts[port_id].Instance = PORT_INSTANCES[port_id];
    port_drivers[port_id] = driver;

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_uart_destruct(cfn_hal_uart_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id < CFN_HAL_UART_PORT_MAX)
    {
        port_drivers[port_id] = NULL;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_UART;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
}
