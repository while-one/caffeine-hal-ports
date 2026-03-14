/**
 * @file cfn_hal_gpio_port.c
 * @brief STM32F4 GPIO HAL Port Implementation.
 */

/* Includes ---------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_gpio_port.h"
#include "cfn_hal_clock_port.h"

/* Private Data -----------------------------------------------------*/

static GPIO_TypeDef * const port_instances[CFN_HAL_GPIO_PORT_MAX] = {
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
#if defined(GPIOJ)
    [CFN_HAL_GPIO_PORT_J] = GPIOJ,
#endif
#if defined(GPIOK)
    [CFN_HAL_GPIO_PORT_K] = GPIOK,
#endif
};

/* VMT Implementations ----------------------------------------------*/

static void low_level_init(cfn_hal_gpio_t *driver)
{
    uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->port;
    /* Map port A-K to clock gating enum */
    cfn_hal_port_clock_enable_gate((cfn_hal_port_peripheral_id_t)(CFN_HAL_PORT_PERIPH_GPIOA + port_id));
}

static cfn_hal_error_code_t port_base_init(cfn_hal_driver_t *base)
{
    low_level_init((cfn_hal_gpio_t *)base);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_base_deinit(cfn_hal_driver_t *base)
{
    cfn_hal_gpio_t *driver = (cfn_hal_gpio_t *)base;
    uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->port;
    HAL_GPIO_DeInit(port_instances[port_id], GPIO_PIN_All);
    return CFN_HAL_ERROR_OK;
}

/* ... standard base stubs ... */
static cfn_hal_error_code_t port_base_power_state_set(cfn_hal_driver_t *base, cfn_hal_power_state_t state) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(state); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_config_set(cfn_hal_driver_t *base, const void *config) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(config); return CFN_HAL_ERROR_NOT_SUPPORTED; }
static cfn_hal_error_code_t port_base_callback_register(cfn_hal_driver_t *base, cfn_hal_callback_t callback, void *user_arg) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(callback); CFN_HAL_UNUSED(user_arg); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_event_enable(cfn_hal_driver_t *base, uint32_t event_mask) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(event_mask); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_event_disable(cfn_hal_driver_t *base, uint32_t event_mask) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(event_mask); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_event_get(cfn_hal_driver_t *base, uint32_t *event_mask) { CFN_HAL_UNUSED(base); if (event_mask) { *event_mask = 0; } return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_error_enable(cfn_hal_driver_t *base, uint32_t error_mask) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(error_mask); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_error_disable(cfn_hal_driver_t *base, uint32_t error_mask) { CFN_HAL_UNUSED(base); CFN_HAL_UNUSED(error_mask); return CFN_HAL_ERROR_OK; }
static cfn_hal_error_code_t port_base_error_get(cfn_hal_driver_t *base, uint32_t *error_mask) { CFN_HAL_UNUSED(base); if (error_mask) { *error_mask = 0; } return CFN_HAL_ERROR_OK; }

/* GPIO Specific Functions */

static cfn_hal_error_code_t port_gpio_pin_config(cfn_hal_gpio_t *driver, const cfn_hal_gpio_pin_config_t *config)
{
    uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->port;
    GPIO_TypeDef *port = port_instances[port_id];
    GPIO_InitTypeDef st_init = {0};

    st_init.Pin = config->pin_mask;

    switch (config->mode)
    {
        case CFN_HAL_GPIO_CONFIG_MODE_INPUT:
            st_init.Mode = GPIO_MODE_INPUT;
            break;
        case CFN_HAL_GPIO_CONFIG_MODE_OUTPUT_PP:
            st_init.Mode = GPIO_MODE_OUTPUT_PP;
            break;
        case CFN_HAL_GPIO_CONFIG_MODE_OUTPUT_OD:
            st_init.Mode = GPIO_MODE_OUTPUT_OD;
            break;
        case CFN_HAL_GPIO_CONFIG_MODE_ALTERNATE_PP:
            st_init.Mode = GPIO_MODE_AF_PP;
            break;
        case CFN_HAL_GPIO_CONFIG_MODE_ALTERNATE_OD:
            st_init.Mode = GPIO_MODE_AF_OD;
            break;
        case CFN_HAL_GPIO_CONFIG_MODE_ANALOG:
            st_init.Mode = GPIO_MODE_ANALOG;
            break;
        default:
            return CFN_HAL_ERROR_BAD_PARAM;
    }

    switch (config->pull)
    {
        case CFN_HAL_GPIO_CONFIG_PULL_NOPULL:
            st_init.Pull = GPIO_NOPULL;
            break;
        case CFN_HAL_GPIO_CONFIG_PULL_PULLUP:
            st_init.Pull = GPIO_PULLUP;
            break;
        case CFN_HAL_GPIO_CONFIG_PULL_PULLDOWN:
            st_init.Pull = GPIO_PULLDOWN;
            break;
        default:
            return CFN_HAL_ERROR_BAD_PARAM;
    }

    switch (config->speed)
    {
        case CFN_HAL_GPIO_CONFIG_SPEED_LOW:
            st_init.Speed = GPIO_SPEED_FREQ_LOW;
            break;
        case CFN_HAL_GPIO_CONFIG_SPEED_MEDIUM:
            st_init.Speed = GPIO_SPEED_FREQ_MEDIUM;
            break;
        case CFN_HAL_GPIO_CONFIG_SPEED_HIGH:
            st_init.Speed = GPIO_SPEED_FREQ_HIGH;
            break;
        case CFN_HAL_GPIO_CONFIG_SPEED_VERY_HIGH:
            st_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            break;
        default:
            return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (config->alternate)
    {
        st_init.Alternate = (uint32_t)(uintptr_t)config->alternate;
    }

    HAL_GPIO_Init(port, &st_init);
    
    /* Set default state */
    if (config->mode == CFN_HAL_GPIO_CONFIG_MODE_OUTPUT_PP || config->mode == CFN_HAL_GPIO_CONFIG_MODE_OUTPUT_OD)
    {
        HAL_GPIO_WritePin(port, (uint16_t)config->pin_mask, (config->default_state == CFN_HAL_GPIO_STATE_HIGH) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }

    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_gpio_pin_read(cfn_hal_gpio_t *driver, cfn_hal_gpio_pin_t pin, cfn_hal_gpio_state_t *state)
{
    uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->port;
    *state = (HAL_GPIO_ReadPin(port_instances[port_id], (uint16_t)pin) == GPIO_PIN_SET) ? CFN_HAL_GPIO_STATE_HIGH : CFN_HAL_GPIO_STATE_LOW;
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_gpio_pin_write(cfn_hal_gpio_t *driver, cfn_hal_gpio_pin_t pin, cfn_hal_gpio_state_t state)
{
    uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->port;
    HAL_GPIO_WritePin(port_instances[port_id], (uint16_t)pin, (state == CFN_HAL_GPIO_STATE_HIGH) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_gpio_pin_toggle(cfn_hal_gpio_t *driver, cfn_hal_gpio_pin_t pin)
{
    uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->port;
    HAL_GPIO_TogglePin(port_instances[port_id], (uint16_t)pin);
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_gpio_port_read(cfn_hal_gpio_t *driver, uint32_t *port_value)
{
    uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->port;
    *port_value = (uint32_t)port_instances[port_id]->IDR;
    return CFN_HAL_ERROR_OK;
}

static cfn_hal_error_code_t port_gpio_port_write(cfn_hal_gpio_t *driver, uint32_t pin_mask, uint32_t port_value)
{
    uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->port;
    GPIO_TypeDef *port = port_instances[port_id];
    /* atomic set/reset using BSRR */
    port->BSRR = ((~port_value & pin_mask) << 16) | (port_value & pin_mask);
    return CFN_HAL_ERROR_OK;
}

/* API --------------------------------------------------------------*/
static const cfn_hal_gpio_api_t gpio_api = {
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
    if ((driver == NULL) || (phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t)(uintptr_t)phy->port;
    if (port_id >= CFN_HAL_GPIO_PORT_MAX || port_instances[port_id] == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    driver->api = &gpio_api;
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_GPIO;
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
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
