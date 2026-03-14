/**
 * @file cfn_hal_dma_port.h
 * @brief STM32F4 DMA HAL Port Implementation Header.
 */

#ifndef CAFFEINE_HAL_PORT_DMA_STM32F4_H
#define CAFFEINE_HAL_PORT_DMA_STM32F4_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_dma.h"

/* Types Enums ------------------------------------------------------*/

/**
 * @brief STM32F4 Physical DMA Stream instances.
 * STM32F4 has 2 DMA controllers with 8 streams each.
 */
typedef enum
{
    CFN_HAL_DMA_PORT_DMA1_STREAM0,
    CFN_HAL_DMA_PORT_DMA1_STREAM1,
    CFN_HAL_DMA_PORT_DMA1_STREAM2,
    CFN_HAL_DMA_PORT_DMA1_STREAM3,
    CFN_HAL_DMA_PORT_DMA1_STREAM4,
    CFN_HAL_DMA_PORT_DMA1_STREAM5,
    CFN_HAL_DMA_PORT_DMA1_STREAM6,
    CFN_HAL_DMA_PORT_DMA1_STREAM7,
    CFN_HAL_DMA_PORT_DMA2_STREAM0,
    CFN_HAL_DMA_PORT_DMA2_STREAM1,
    CFN_HAL_DMA_PORT_DMA2_STREAM2,
    CFN_HAL_DMA_PORT_DMA2_STREAM3,
    CFN_HAL_DMA_PORT_DMA2_STREAM4,
    CFN_HAL_DMA_PORT_DMA2_STREAM5,
    CFN_HAL_DMA_PORT_DMA2_STREAM6,
    CFN_HAL_DMA_PORT_DMA2_STREAM7,
    CFN_HAL_DMA_PORT_MAX
} cfn_hal_dma_port_t;

/**
 * @brief STM32F4 DMA Channel mapping (Request Selection).
 */
typedef enum
{
    CFN_HAL_DMA_CHANNEL_0 = 0,
    CFN_HAL_DMA_CHANNEL_1,
    CFN_HAL_DMA_CHANNEL_2,
    CFN_HAL_DMA_CHANNEL_3,
    CFN_HAL_DMA_CHANNEL_4,
    CFN_HAL_DMA_CHANNEL_5,
    CFN_HAL_DMA_CHANNEL_6,
    CFN_HAL_DMA_CHANNEL_7
} cfn_hal_dma_channel_t;

/* Functions prototypes ---------------------------------------------*/

cfn_hal_error_code_t
cfn_hal_dma_construct(cfn_hal_dma_t *driver, const cfn_hal_dma_config_t *config, const cfn_hal_dma_phy_t *phy);

cfn_hal_error_code_t cfn_hal_dma_destruct(cfn_hal_dma_t *driver);

#ifdef __cplusplus
}
#endif

#endif /* CAFFEINE_HAL_PORT_DMA_STM32F4_H */
