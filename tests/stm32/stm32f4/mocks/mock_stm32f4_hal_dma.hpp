#ifndef MOCK_STM32F4_HAL_DMA_HPP
#define MOCK_STM32F4_HAL_DMA_HPP

#include <gmock/gmock.h>

extern "C" {
#include "stm32f4xx_hal.h"
}

class MockStm32F4HalDma {
public:
    virtual ~MockStm32F4HalDma() = default;

    virtual HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma) = 0;
    virtual HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *hdma) = 0;
    virtual HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength) = 0;
    virtual HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *hdma) = 0;
    virtual uint32_t HAL_DMA_GetError(DMA_HandleTypeDef *hdma) = 0;
    virtual void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma) = 0;

    static MockStm32F4HalDma* GetInstance();
    static void SetInstance(MockStm32F4HalDma* instance);

private:
    static MockStm32F4HalDma* instance_;
};

#endif // MOCK_STM32F4_HAL_DMA_HPP
