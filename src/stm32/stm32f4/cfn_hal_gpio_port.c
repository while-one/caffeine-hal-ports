/**
 * @file cfn_hal_gpio_port.c
 * @brief STM32F4 GPIO HAL Port Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_gpio_port.h"
#include "cfn_hal_clock_port.h"
#include "cfn_hal_stm32_error.h"

/* Private Data -----------------------------------------------------*/

static GPIO_TypeDef *const PORT_INSTANCES[CFN_HAL_GPIO_PORT_MAX] = {
#if defined(GPIOA)
    [CFN_HAL_GPIO_PORT_A] = GPIOA,
#endif
#if defined(GPIOB)
    [CFN_HAL_GPIO_PORT_B] = GPIOB,
#endif
#if defined(GPIOC)
    [CFN_HAL_GPIO_PORT_C] = GPIOC,
#endif
#if defined(GPIOD)
    [CFN_HAL_GPIO_PORT_D] = GPIOD,
#endif
#if defined(GPIOE)
    [CFN_HAL_GPIO_PORT_E] = GPIOE,
#endif
#if defined(GPIOF)
    [CFN_HAL_GPIO_PORT_F] = GPIOF,
#endif
#if defined(GPIOG)
    [CFN_HAL_GPIO_PORT_G] = GPIOG,
#endif
#if defined(GPIOH)
    [CFN_HAL_GPIO_PORT_H] = GPIOH,
#endif
#if defined(GPIOI)
    [CFN_HAL_GPIO_PORT_I] = GPIOI,
#endif
};

/* VMT Implementations ----------------------------------------------*/

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    cfn_hal_gpio_t *driver = (cfn_hal_gpio_t *) base;
    uint32_t        port_id = (uint32_t) (uintptr_t) driver->phy->port;

    /* 1. Enable Clock */
    cfn_hal_port_clock_enable_gate((cfn_hal_port_peripheral_id_t) (CFN_HAL_PORT_PERIPH_GPIOA + port_id));

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_gpio_t *driver = (cfn_hal_gpio_t *) base;
    uint32_t        port_id = (uint32_t) (uintptr_t) driver->phy->port;
    HAL_GPIO_DeInit(PORT_INSTANCES[port_id], GPIO_PIN_All);
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

static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask)
{
    CFN_HAL_UNUSED(base);
    if (error_mask)
    {
        *error_mask = 0;
    }
    return CFN_HAL_ERROR_OK;
}

/* GPIO Specific Functions */

static cfn_hal_error_code_t port_gpio_pin_config(cfn_hal_gpio_t *port, const cfn_hal_gpio_pin_config_t *pin_cfg)
{
    uint32_t         port_id = (uint32_t) (uintptr_t) port->phy->port;
    GPIO_InitTypeDef st_cfg = { 0 };

    st_cfg.Pin = pin_cfg->pin_mask;

    switch (pin_cfg->mode)
    {
        case CFN_HAL_GPIO_CONFIG_MODE_INPUT:
            st_cfg.Mode = GPIO_MODE_INPUT;
            break;
        case CFN_HAL_GPIO_CONFIG_MODE_OUTPUT_PP:
            st_cfg.Mode = GPIO_MODE_OUTPUT_PP;
            break;
        case CFN_HAL_GPIO_CONFIG_MODE_OUTPUT_OD:
            st_cfg.Mode = GPIO_MODE_OUTPUT_OD;
            break;
        case CFN_HAL_GPIO_CONFIG_MODE_ALTERNATE_PP:
            st_cfg.Mode = GPIO_MODE_AF_PP;
            st_cfg.Alternate = (uint32_t) (uintptr_t) pin_cfg->alternate;
            break;
        case CFN_HAL_GPIO_CONFIG_MODE_ALTERNATE_OD:
            st_cfg.Mode = GPIO_MODE_AF_OD;
            st_cfg.Alternate = (uint32_t) (uintptr_t) pin_cfg->alternate;
            break;
        case CFN_HAL_GPIO_CONFIG_MODE_ANALOG:
            st_cfg.Mode = GPIO_MODE_ANALOG;
            break;
    }

    switch (pin_cfg->pull)
    {
        case CFN_HAL_GPIO_CONFIG_PULL_NOPULL:
            st_cfg.Pull = GPIO_NOPULL;
            break;
        case CFN_HAL_GPIO_CONFIG_PULL_PULLUP:
            st_cfg.Pull = GPIO_PULLUP;
            break;
        case CFN_HAL_GPIO_CONFIG_PULL_PULLDOWN:
            st_cfg.Pull = GPIO_PULLDOWN;
            break;
    }

    switch (pin_cfg->speed)
    {
        case CFN_HAL_GPIO_CONFIG_SPEED_LOW:
            st_cfg.Speed = GPIO_SPEED_FREQ_LOW;
            break;
        case CFN_HAL_GPIO_CONFIG_SPEED_MEDIUM:
            st_cfg.Speed = GPIO_SPEED_FREQ_MEDIUM;
            break;
        case CFN_HAL_GPIO_CONFIG_SPEED_HIGH:
            st_cfg.Speed = GPIO_SPEED_FREQ_HIGH;
            break;
        case CFN_HAL_GPIO_CONFIG_SPEED_VERY_HIGH:
            st_cfg.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            break;
    }

    HAL_GPIO_Init(PORT_INSTANCES[port_id], &st_cfg);

    if ((pin_cfg->mode == CFN_HAL_GPIO_CONFIG_MODE_OUTPUT_PP) || (pin_cfg->mode == CFN_HAL_GPIO_CONFIG_MODE_OUTPUT_OD))
    {
        HAL_GPIO_WritePin(PORT_INSTANCES[port_id],
                          pin_cfg->pin_mask,
                          (pin_cfg->default_state == CFN_HAL_GPIO_STATE_HIGH) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_gpio_pin_write(cfn_hal_gpio_t *port, cfn_hal_gpio_pin_t pin, cfn_hal_gpio_state_t state)
{
    uint32_t port_id = (uint32_t) (uintptr_t) port->phy->port;
    HAL_GPIO_WritePin(PORT_INSTANCES[port_id], pin, (state == CFN_HAL_GPIO_STATE_HIGH) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t
port_gpio_pin_read(cfn_hal_gpio_t *port, cfn_hal_gpio_pin_t pin, cfn_hal_gpio_state_t *state)
{
    uint32_t port_id = (uint32_t) (uintptr_t) port->phy->port;
    if (!state)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *state = (HAL_GPIO_ReadPin(PORT_INSTANCES[port_id], pin) == GPIO_PIN_SET) ? CFN_HAL_GPIO_STATE_HIGH
                                                                              : CFN_HAL_GPIO_STATE_LOW;
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_gpio_pin_toggle(cfn_hal_gpio_t *port, cfn_hal_gpio_pin_t pin)
{
    uint32_t port_id = (uint32_t) (uintptr_t) port->phy->port;
    HAL_GPIO_TogglePin(PORT_INSTANCES[port_id], pin);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_gpio_port_write(cfn_hal_gpio_t *port, uint32_t pin_mask, uint32_t port_value)
{
    uint32_t port_id = (uint32_t) (uintptr_t) port->phy->port;
    MODIFY_REG(PORT_INSTANCES[port_id]->ODR, pin_mask, port_value & pin_mask);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_gpio_port_read(cfn_hal_gpio_t *port, uint32_t *port_value)
{
    uint32_t port_id = (uint32_t) (uintptr_t) port->phy->port;
    if (!port_value)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *port_value = PORT_INSTANCES[port_id]->IDR;
    return CFN_HAL_ERROR_OK;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_gpio_api_t GPIO_API = {
    .base = {
        .init = port_base_init,
        .deinit = port_base_deinit,
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
    .pin_config = port_gpio_pin_config,
    .pin_read = port_gpio_pin_read,
    .pin_write = port_gpio_pin_write,
    .pin_toggle = port_gpio_pin_toggle,
    .port_read = port_gpio_port_read,
    .port_write = port_gpio_port_write
};

/* Instantiation ----------------------------------------------------*/

cfn_hal_error_code_t cfn_hal_gpio_construct(cfn_hal_gpio_t *driver, void *config, const cfn_hal_gpio_phy_t *phy)
{
    CFN_HAL_UNUSED(config);
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t) (uintptr_t) phy->port;
    if (port_id >= CFN_HAL_GPIO_PORT_MAX || PORT_INSTANCES[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &GPIO_API;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_GPIO;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = NULL;
    driver->phy = phy;

    return CFN_HAL_ERROR_OK;
}

cfn_hal_error_code_t cfn_hal_gpio_destruct(cfn_hal_gpio_t *driver)
{
    if (driver == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = NULL;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_GPIO;
    driver->base.status = CFN_HAL_DRIVER_STATUS_UNKNOWN;
    driver->config = NULL;
    driver->phy = NULL;

    return CFN_HAL_ERROR_OK;
}
