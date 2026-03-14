/**
 * @file cfn_hal_can_port.c
 * @brief STM32F4 CAN HAL Port Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_can.h"
#include "cfn_hal_can_port.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_stm32_error.h"

/* Private Data -----------------------------------------------------*/

static CAN_TypeDef *const PORT_INSTANCES[CFN_HAL_CAN_PORT_MAX] = {
#if defined(CAN1)
    [CFN_HAL_CAN_PORT_CAN1] = CAN1,
#endif
#if defined(CAN2)
    [CFN_HAL_CAN_PORT_CAN2] = CAN2,
#endif
#if defined(CAN3)
    [CFN_HAL_CAN_PORT_CAN3] = CAN3,
#endif
};

static CAN_HandleTypeDef port_hcans[CFN_HAL_CAN_PORT_MAX];

/* VMT Implementations ----------------------------------------------*/

static void low_level_init(cfn_hal_can_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    /* 1. Enable Clock */
    cfn_hal_port_clock_enable_gate((cfn_hal_port_peripheral_id_t) (CFN_HAL_PORT_PERIPH_CAN1 + port_id));

    /* 2. Initialize Pins */
    if (driver->phy->tx)
    {
        (void) cfn_hal_base_init((cfn_hal_driver_t *) driver->phy->tx, CFN_HAL_PERIPHERAL_TYPE_GPIO);
    }
    if (driver->phy->rx)
    {
        (void) cfn_hal_base_init((cfn_hal_driver_t *) driver->phy->rx, CFN_HAL_PERIPHERAL_TYPE_GPIO);
    }
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_can_t     *driver = (cfn_hal_can_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    CAN_HandleTypeDef *hcan = &port_hcans[port_id];

    low_level_init(driver);

    hcan->Instance = PORT_INSTANCES[port_id];

    /* Simple default bit timing for F4 @ 42MHz APB1, 500kbps */
    hcan->Init.Prescaler = 3;
    hcan->Init.Mode = CAN_MODE_NORMAL;
    hcan->Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan->Init.TimeSeg1 = CAN_BS1_11TQ;
    hcan->Init.TimeSeg2 = CAN_BS2_2TQ;
    hcan->Init.TimeTriggeredMode = DISABLE;
    hcan->Init.AutoBusOff = DISABLE;
    hcan->Init.AutoWakeUp = DISABLE;
    hcan->Init.AutoRetransmission = ENABLE;
    hcan->Init.ReceiveFifoLocked = DISABLE;
    hcan->Init.TransmitFifoPriority = DISABLE;

    return cfn_hal_stm32_map_error(HAL_CAN_Init(hcan));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_can_t *driver = (cfn_hal_can_t *) base;
    uint32_t       port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_CAN_DeInit(&port_hcans[port_id]));
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
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(event_mask);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(event_mask);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask)
{
    CFN_HAL_UNUSED(base);
    if (event_mask)
    {
        *event_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(error_mask);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(error_mask);
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

/* CAN Specific Functions */

static cfn_hal_error_code_t port_can_transmit(cfn_hal_can_t *driver, const cfn_hal_can_msg_t *msg, uint32_t timeout)
{
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    CAN_HandleTypeDef  *hcan = &port_hcans[port_id];
    CAN_TxHeaderTypeDef st_header = { 0 };
    uint32_t            tx_mailbox;

    st_header.StdId = msg->id;
    st_header.DLC = msg->dlc;
    st_header.IDE = CAN_ID_STD;
    st_header.RTR = CAN_RTR_DATA;

    if (HAL_CAN_AddTxMessage(hcan, &st_header, (uint8_t *) msg->data, &tx_mailbox) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }

    uint32_t tickstart = HAL_GetTick();
    while (HAL_CAN_IsTxMessagePending(hcan, tx_mailbox))
    {
        if ((timeout == 0U) || ((HAL_GetTick() - tickstart) > timeout))
        {
            return CFN_HAL_ERROR_TIMING_TIMEOUT;
        }
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_can_receive(cfn_hal_can_t *driver, cfn_hal_can_msg_t *msg, uint32_t timeout)
{
    uint32_t            port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    CAN_HandleTypeDef  *hcan = &port_hcans[port_id];
    CAN_RxHeaderTypeDef st_header = { 0 };

    uint32_t tickstart = HAL_GetTick();
    while (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) == 0)
    {
        if ((timeout == 0U) || ((HAL_GetTick() - tickstart) > timeout))
        {
            return CFN_HAL_ERROR_TIMING_TIMEOUT;
        }
    }

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &st_header, msg->data) != HAL_OK)
    {
        return CFN_HAL_ERROR_FAIL;
    }

    msg->id = st_header.StdId;
    msg->dlc = st_header.DLC;

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_can_add_filter(cfn_hal_can_t *driver, const cfn_hal_can_filter_t *filter)
{
    uint32_t          port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    CAN_FilterTypeDef st_filter = { 0 };

    st_filter.FilterIdHigh = (uint16_t) (filter->filter_id >> 16);
    st_filter.FilterIdLow = (uint16_t) (filter->filter_id & 0xFFFF);
    st_filter.FilterMaskIdHigh = (uint16_t) (filter->filter_mask >> 16);
    st_filter.FilterMaskIdLow = (uint16_t) (filter->filter_mask & 0xFFFF);
    st_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    st_filter.FilterBank = 0;
    st_filter.FilterMode = CAN_FILTERMODE_IDMASK;
    st_filter.FilterScale = CAN_FILTERSCALE_32BIT;
    st_filter.FilterActivation = ENABLE;

    return cfn_hal_stm32_map_error(HAL_CAN_ConfigFilter(&port_hcans[port_id], &st_filter));
}

/* API --------------------------------------------------------------*/
static const cfn_hal_can_api_t CAN_API = {
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
    .transmit = port_can_transmit,
    .receive = port_can_receive,
    .add_filter = port_can_add_filter
};

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_can_construct(cfn_hal_can_t *driver, const cfn_hal_can_config_t *config, const cfn_hal_can_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_CAN_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &CAN_API;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_CAN;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;

    port_hcans[port_id].Instance = PORT_INSTANCES[port_id];

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_can_destruct(cfn_hal_can_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_CAN;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
}
