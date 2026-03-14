/**
 * @file cfn_hal_dma_port.h
 * @brief DMA HAL Template Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_DMA_H
#define CAFFEINE_HAL_PORT_DMA_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfn_hal_dma.h"

cfn_hal_error_code_t cfn_hal_dma_construct(cfn_hal_dma_t *driver, const cfn_hal_dma_config_t *config, const cfn_hal_dma_phy_t *phy);
cfn_hal_error_code_t cfn_hal_dma_destruct(cfn_hal_dma_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_DMA_H */
