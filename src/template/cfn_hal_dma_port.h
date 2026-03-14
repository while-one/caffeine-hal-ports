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

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_dma.h"

/* Functions prototypes ---------------------------------------------*/

/**
 * @brief Construct the dma driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the configuration structure.
 * @param phy Pointer to the physical mapping structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_dma_construct(cfn_hal_dma_t *driver,
                                            const cfn_hal_dma_config_t *config,
                                            const cfn_hal_dma_phy_t *phy);

/**
 * @brief Destruct the dma driver.
 *
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
cfn_hal_error_code_t cfn_hal_dma_destruct(cfn_hal_dma_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_DMA_H */
