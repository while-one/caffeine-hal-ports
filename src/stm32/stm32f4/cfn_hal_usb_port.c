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
 * @file cfn_hal_usb_port.c
 * @brief STM32F4 USB HAL Port Implementation
 */

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_usb_port.h"
#include "cfn_hal_clock.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_stm32_error.h"
#include "cfn_hal_usb.h"
#include "stm32f4xx_hal.h"
#include <string.h>

/* Private Prototypes ----------------------------------------------*/
void OTG_FS_IRQHandler(void);
void OTG_HS_IRQHandler(void);

/* Private Prototypes ----------------------------------------------*/
#if defined(HAL_PCD_MODULE_ENABLED)

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

static uint32_t get_port_id_from_handle(PCD_HandleTypeDef *hpcd)
{
    if ((hpcd < &port_hpcds[0]) || (hpcd >= &port_hpcds[CFN_HAL_USB_PORT_MAX]))
    {
        return UINT32_MAX;
    }
    return (uint32_t) (hpcd - port_hpcds);
}

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t low_level_init(cfn_hal_usb_t *driver)
{
    if (driver == NULL || driver->phy == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    struct cfn_hal_clock_s *clk = driver->base.clock_driver;
    if (clk == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;

    if (port_id >= CFN_HAL_USB_PORT_MAX)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (port_id == (uint32_t) CFN_HAL_USB_PORT_OTG_FS)
    {
        cfn_hal_clock_enable_gate((cfn_hal_clock_t *) clk, driver->base.peripheral_id);
    }
    else
    {
        cfn_hal_clock_enable_gate((cfn_hal_clock_t *) clk, driver->base.peripheral_id);
    }

    /* 2. Initialize Pins */
    if (driver->phy->dp)
    {
        (void) cfn_hal_gpio_init(driver->phy->dp->port);
    }
    if (driver->phy->dm)
    {
        (void) cfn_hal_gpio_init(driver->phy->dm->port);
    }
    if (driver->phy->id)
    {
        (void) cfn_hal_gpio_init(driver->phy->id->port);
    }
    if (driver->phy->vbus)
    {
        (void) cfn_hal_gpio_init(driver->phy->vbus->port);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    if (base == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_usb_t     *driver  = (cfn_hal_usb_t *) base;
    uint32_t           port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    PCD_HandleTypeDef *hpcd    = &port_hpcds[port_id];

    cfn_hal_error_code_t error = low_level_init(driver);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }

    hpcd->Instance                 = PORT_INSTANCES[port_id];
    hpcd->Init.dev_endpoints       = 7;
    hpcd->Init.speed               = PCD_SPEED_FULL;
    hpcd->Init.phy_itface          = PCD_PHY_EMBEDDED;
    hpcd->Init.Sof_enable          = DISABLE;
    hpcd->Init.low_power_enable    = DISABLE;
    hpcd->Init.lpm_enable          = DISABLE;
    hpcd->Init.vbus_sensing_enable = DISABLE;
    hpcd->Init.use_dedicated_ep1   = DISABLE;

    return cfn_hal_stm32_map_error(HAL_PCD_Init(hpcd));
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_usb_t *driver  = (cfn_hal_usb_t *) base;
    uint32_t       port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    return cfn_hal_stm32_map_error(HAL_PCD_DeInit(&port_hpcds[port_id]));
}

static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config)
{
    CFN_HAL_UNUSED(config);
    return port_base_init(base);
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
    uint32_t port_id = get_port_id_from_handle(hpcd);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
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
    uint32_t port_id = get_port_id_from_handle(hpcd);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
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
    uint32_t port_id = get_port_id_from_handle(hpcd);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
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
    uint32_t port_id = get_port_id_from_handle(hpcd);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
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
    uint32_t port_id = get_port_id_from_handle(hpcd);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
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
    uint32_t port_id = get_port_id_from_handle(hpcd);
    if ((port_id != UINT32_MAX) && (port_drivers[port_id] != NULL))
    {
        cfn_hal_usb_t *driver = port_drivers[port_id];
        if (driver->cb != NULL)
        {
            driver->cb(driver, CFN_HAL_USB_EVENT_RESUME, 0, 0, driver->cb_user_arg);
        }
    }
}

/* Raw ISR Handlers -------------------------------------------------*/

#ifndef CFN_HAL_PORT_DISABLE_IRQ_USB
void OTG_FS_IRQHandler(void); // NOLINT(readability-identifier-naming)
void OTG_HS_IRQHandler(void); // NOLINT(readability-identifier-naming)

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

#endif /* CFN_HAL_PORT_DISABLE_IRQ_USB */

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
    uint32_t port_id    = (uint32_t) (uintptr_t) driver->phy->instance;
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
    PCD_HandleTypeDef *hpcd    = &port_hpcds[port_id];
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
    .base =
        {
            .init = port_base_init,
            .deinit = port_base_deinit,
            .power_state_set = NULL,
            .config_set = port_base_config_set,
            .config_validate = NULL,
            .callback_register = NULL,
            .event_enable = NULL,
            .event_disable = NULL,
            .event_get = port_base_event_get,
            .error_enable = NULL,
            .error_disable = NULL,
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
    .get_rx_data_size = port_usb_get_rx_data_size};

#endif /* HAL_PCD_MODULE_ENABLED */

/* Instantiation ----------------------------------------------------*/

/**
 * @brief Constructs a new USB driver instance.
 * @param driver Pointer to the driver structure to construct.
 * @param config Pointer to the USB configuration.
 * @param phy Pointer to the physical USB mapping.
 * @param clock Pointer to the clock driver instance.
 * @param callback User callback function.
 * @param user_arg User argument for the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_usb_construct(cfn_hal_usb_t *driver, const cfn_hal_usb_config_t *config, const cfn_hal_usb_phy_t *phy, struct cfn_hal_clock_s *clock, cfn_hal_usb_callback_t callback, void *user_arg)
{
#ifdef HAL_PCD_MODULE_ENABLED
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->instance;
    if (port_id >= CFN_HAL_USB_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    cfn_hal_usb_populate(driver, clock, &USB_API, phy, config, callback, user_arg);

    port_hpcds[port_id].Instance = PORT_INSTANCES[port_id];
    port_drivers[port_id]        = driver;

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

/**
 * @brief Destructs an existing USB driver instance.
 * @param driver Pointer to the driver instance to destruct.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_usb_destruct(cfn_hal_usb_t *driver)
{
#ifdef HAL_PCD_MODULE_ENABLED
    if (driver == NULL || driver->phy == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) driver->phy->instance;
    if (port_id < CFN_HAL_USB_PORT_MAX)
    {
        port_drivers[port_id] = NULL;
    }

    driver->config = NULL;
    driver->phy    = NULL;
    return CFN_HAL_ERROR_OK;
#else
    CFN_HAL_UNUSED(driver);
    return CFN_HAL_ERROR_NOT_SUPPORTED;
#endif
}
