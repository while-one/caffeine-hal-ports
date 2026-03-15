/**
 * @file cfn_hal_usb_port.c
 * @brief STM32F4 USB HAL Port Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_usb.h"
#include "cfn_hal_usb_port.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_stm32_error.h"
#include <string.h>

/* Private Data -----------------------------------------------------*/

static USB_OTG_GlobalTypeDef *const PORT_INSTANCES[CFN_HAL_USB_PORT_MAX] = {
#if defined(USB_OTG_FS)
    [CFN_HAL_USB_PORT_OTG_FS] = USB_OTG_FS,
#endif
#if defined(USB_OTG_HS)
    [CFN_HAL_USB_PORT_OTG_HS] = USB_OTG_HS,
#endif
};

static PCD_HandleTypeDef port_hpcds[CFN_HAL_USB_PORT_MAX];
static cfn_hal_usb_t    *port_drivers[CFN_HAL_USB_PORT_MAX];

/* Internal Helpers -------------------------------------------------*/

static int32_t get_port_id_from_handle(PCD_HandleTypeDef *hpcd)
{
    for (uint32_t i = 0; i < CFN_HAL_USB_PORT_MAX; i++)
    {
        if (&port_hpcds[i] == hpcd)
        {
            return (int32_t) i;
        }
    }
    return -1;
}

/* VMT Implementations ----------------------------------------------*/

static void low_level_init(cfn_hal_usb_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id == (uint32_t) CFN_HAL_USB_PORT_OTG_FS)
    {
        cfn_hal_port_clock_enable_gate(CFN_HAL_PORT_PERIPH_USB_OTG_FS);
    }
    else
    {
        cfn_hal_port_clock_enable_gate(CFN_HAL_PORT_PERIPH_USB_OTG_HS);
    }
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_usb_t     *driver = (cfn_hal_usb_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    PCD_HandleTypeDef *hpcd = &port_hpcds[port_id];

    low_level_init(driver);

    hpcd->Instance = PORT_INSTANCES[port_id];
    hpcd->Init.dev_endpoints = 7;
    hpcd->Init.speed = PCD_SPEED_FULL;
    hpcd->Init.phy_itface = PCD_PHY_EMBEDDED;
    hpcd->Init.Sof_enable = DISABLE;
    hpcd->Init.low_power_enable = DISABLE;
    hpcd->Init.lpm_enable = DISABLE;
    hpcd->Init.vbus_sensing_enable = DISABLE;
    hpcd->Init.use_dedicated_ep1 = DISABLE;

    return cfn_hal_stm32_map_error(HAL_PCD_Init(hpcd));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_usb_t *driver = (cfn_hal_usb_t *) base;
    uint32_t       port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_PCD_DeInit(&port_hpcds[port_id]));
}

static cfn_hal_error_code_t port_base_power_state_set(cfn_hal_driver_t *base, cfn_hal_power_state_t state)
{
    CFN_HAL_UNUSED(base);
    CFN_HAL_UNUSED(state);
    return CFN_HAL_ERROR_OK;
}
static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
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
    if (event_mask != NULL)
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
    if (error_mask != NULL)
    {
        *error_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

/* ST HAL Callback Overrides ----------------------------------------*/

void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
    int32_t port_id = get_port_id_from_handle(hpcd);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_usb_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_USB_EVENT_RESET, 0, 0, driver->cb_user_arg);
        }
    }
}

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
    int32_t port_id = get_port_id_from_handle(hpcd);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_usb_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_USB_EVENT_SETUP_READY, 0, 0, driver->cb_user_arg);
        }
    }
}

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    int32_t port_id = get_port_id_from_handle(hpcd);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_usb_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_USB_EVENT_EP_DATA_OUT, 0, epnum, driver->cb_user_arg);
        }
    }
}

void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    int32_t port_id = get_port_id_from_handle(hpcd);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_usb_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_USB_EVENT_EP_DATA_IN, 0, epnum | 0x80, driver->cb_user_arg);
        }
    }
}

void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
    int32_t port_id = get_port_id_from_handle(hpcd);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_usb_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_USB_EVENT_SUSPEND, 0, 0, driver->cb_user_arg);
        }
    }
}

void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
    int32_t port_id = get_port_id_from_handle(hpcd);
    if ((port_id >= 0) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_usb_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_USB_EVENT_RESUME, 0, 0, driver->cb_user_arg);
        }
    }
}

/* Raw ISR Handlers -------------------------------------------------*/

#if defined(USB_OTG_FS)
void OTG_FS_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_PCD_IRQHandler(&port_hpcds[CFN_HAL_USB_PORT_OTG_FS]);
}
#endif

#if defined(USB_OTG_HS)
void OTG_HS_IRQHandler(void) // NOLINT(readability-identifier-naming)
{
    HAL_PCD_IRQHandler(&port_hpcds[CFN_HAL_USB_PORT_OTG_HS]);
}
#endif

/* USB Specific Functions */

static cfn_hal_error_code_t port_usb_start(cfn_hal_usb_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_PCD_Start(&port_hpcds[port_id]));
}

static cfn_hal_error_code_t port_usb_stop(cfn_hal_usb_t *driver)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_PCD_Stop(&port_hpcds[port_id]));
}

static cfn_hal_error_code_t port_usb_set_address(cfn_hal_usb_t *driver, uint8_t address)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_PCD_SetAddress(&port_hpcds[port_id], address));
}

static cfn_hal_error_code_t
port_usb_ep_open(cfn_hal_usb_t *driver, uint8_t ep_addr, cfn_hal_usb_ep_type_t ep_type, uint16_t ep_mps)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    uint8_t  st_ep_type = EP_TYPE_CTRL;
    switch (ep_type)
    {
        case CFN_HAL_USB_EP_TYPE_CTRL:
            st_ep_type = EP_TYPE_CTRL;
            break;
        case CFN_HAL_USB_EP_TYPE_ISOC:
            st_ep_type = EP_TYPE_ISOC;
            break;
        case CFN_HAL_USB_EP_TYPE_BULK:
            st_ep_type = EP_TYPE_BULK;
            break;
        case CFN_HAL_USB_EP_TYPE_INTR:
            st_ep_type = EP_TYPE_INTR;
            break;
    }
    return cfn_hal_stm32_map_error(HAL_PCD_EP_Open(&port_hpcds[port_id], ep_addr, ep_mps, st_ep_type));
}

static cfn_hal_error_code_t port_usb_ep_close(cfn_hal_usb_t *driver, uint8_t ep_addr)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_PCD_EP_Close(&port_hpcds[port_id], ep_addr));
}

static cfn_hal_error_code_t
port_usb_ep_transmit(cfn_hal_usb_t *driver, uint8_t ep_addr, const uint8_t *data, size_t length)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(
        HAL_PCD_EP_Transmit(&port_hpcds[port_id], ep_addr, (uint8_t *) data, (uint32_t) length));
}

static cfn_hal_error_code_t port_usb_ep_receive(cfn_hal_usb_t *driver, uint8_t ep_addr, uint8_t *buffer, size_t length)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_PCD_EP_Receive(&port_hpcds[port_id], ep_addr, buffer, (uint32_t) length));
}

static cfn_hal_error_code_t port_usb_ep_stall(cfn_hal_usb_t *driver, uint8_t ep_addr, bool stall)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (stall)
    {
        return cfn_hal_stm32_map_error(HAL_PCD_EP_SetStall(&port_hpcds[port_id], ep_addr));
    }
    else
    {
        return cfn_hal_stm32_map_error(HAL_PCD_EP_ClrStall(&port_hpcds[port_id], ep_addr));
    }
}

static cfn_hal_error_code_t port_usb_read_setup_packet(cfn_hal_usb_t *driver, uint8_t *buffer)
{
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    PCD_HandleTypeDef *hpcd = &port_hpcds[port_id];
    memcpy(buffer, hpcd->Setup, 8);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_usb_get_rx_data_size(cfn_hal_usb_t *driver, uint8_t ep_addr, size_t *size)
{
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (size == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *size = (size_t) HAL_PCD_EP_GetRxCount(&port_hpcds[port_id], ep_addr);
    return CFN_HAL_ERROR_OK;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_usb_api_t USB_API = {
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
    .start = port_usb_start,
    .stop = port_usb_stop,
    .set_address = port_usb_set_address,
    .ep_open = port_usb_ep_open,
    .ep_close = port_usb_ep_close,
    .ep_transmit = port_usb_ep_transmit,
    .ep_receive = port_usb_ep_receive,
    .ep_stall = port_usb_ep_stall,
    .read_setup_packet = port_usb_read_setup_packet,
    .get_rx_data_size = port_usb_get_rx_data_size
};

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_usb_construct(cfn_hal_usb_t *driver, const cfn_hal_usb_config_t *config, const cfn_hal_usb_phy_t *phy)
{
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_USB_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &USB_API;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_USB;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;

    port_hpcds[port_id].Instance = PORT_INSTANCES[port_id];
    port_drivers[port_id] = driver;

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_usb_destruct(cfn_hal_usb_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id < CFN_HAL_USB_PORT_MAX)
    {
        port_drivers[port_id] = NULL;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_USB;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
}
