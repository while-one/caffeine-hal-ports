#include "mock_stm32f4_hal_dma.hpp"

MockStm32F4HalDma *MockStm32F4HalDma::instance_ = nullptr;

MockStm32F4HalDma *MockStm32F4HalDma::GetInstance()
{
    return instance_;
}

void MockStm32F4HalDma::SetInstance(MockStm32F4HalDma *instance)
{
    instance_ = instance;
}

extern "C"
{
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma)
{
    if (MockStm32F4HalDma::GetInstance())
    {
        return MockStm32F4HalDma::GetInstance()->HAL_DMA_Init(hdma);
    }
    return HAL_OK;
}

HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *hdma)
{
    if (MockStm32F4HalDma::GetInstance())
    {
        return MockStm32F4HalDma::GetInstance()->HAL_DMA_DeInit(hdma);
    }
    return HAL_OK;
}

HAL_StatusTypeDef
HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
    if (MockStm32F4HalDma::GetInstance())
    {
        return MockStm32F4HalDma::GetInstance()->HAL_DMA_Start_IT(hdma, SrcAddress, DstAddress, DataLength);
    }
    return HAL_OK;
}

HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *hdma)
{
    if (MockStm32F4HalDma::GetInstance())
    {
        return MockStm32F4HalDma::GetInstance()->HAL_DMA_Abort_IT(hdma);
    }
    return HAL_OK;
}

uint32_t HAL_DMA_GetError(DMA_HandleTypeDef *hdma)
{
    if (MockStm32F4HalDma::GetInstance())
    {
        return MockStm32F4HalDma::GetInstance()->HAL_DMA_GetError(hdma);
    }
    return 0;
}

void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma)
{
    if (MockStm32F4HalDma::GetInstance())
    {
        MockStm32F4HalDma::GetInstance()->HAL_DMA_IRQHandler(hdma);
    }
}

} // extern "C"
