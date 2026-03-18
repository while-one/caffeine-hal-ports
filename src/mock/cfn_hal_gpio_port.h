#ifndef CAFFEINE_HAL_PORT_GPIO_MOCK_H
#define CAFFEINE_HAL_PORT_GPIO_MOCK_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_gpio.h"

typedef enum
{
    CFN_HAL_GPIO_PORT_A,
    CFN_HAL_GPIO_PORT_MAX
} cfn_hal_gpio_port_t;

static inline cfn_hal_error_code_t cfn_hal_gpio_construct(cfn_hal_gpio_t *driver, void *config, const cfn_hal_gpio_phy_t *phy)
{
    CFN_HAL_UNUSED(driver);
    CFN_HAL_UNUSED(config);
    CFN_HAL_UNUSED(phy);
    return CFN_HAL_ERROR_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_GPIO_MOCK_H */
