/**
 * @file cfn_hal_irq_port.h
 * @brief IRQ HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_IRQ_H
#define CAFFEINE_HAL_PORT_IRQ_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_irq.h"

cfn_hal_error_code_t cfn_hal_irq_construct(cfn_hal_irq_t *driver, const cfn_hal_irq_config_t *config, const cfn_hal_irq_phy_t *phy);
cfn_hal_error_code_t cfn_hal_irq_destruct(cfn_hal_irq_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_IRQ_H */
