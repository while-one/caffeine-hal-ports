#ifndef MOCK_STM32F4_HAL_UART_HPP
#define MOCK_STM32F4_HAL_UART_HPP

#include <gmock/gmock.h>

extern "C" {
#include "stm32f4xx_hal.h"
}

class MockStm32F4HalUart {
public:
    virtual ~MockStm32F4HalUart() = default;

    virtual HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart) = 0;
    virtual HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart) = 0;
    virtual void HAL_UART_IRQHandler(UART_HandleTypeDef *huart) = 0;
    virtual HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout) = 0;
    virtual HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout) = 0;
    virtual HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) = 0;
    virtual HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size) = 0;
    virtual HAL_StatusTypeDef HAL_UART_AbortTransmit_IT(UART_HandleTypeDef *huart) = 0;
    virtual HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) = 0;
    virtual HAL_StatusTypeDef HAL_UART_AbortReceive_IT(UART_HandleTypeDef *huart) = 0;

    static MockStm32F4HalUart* GetInstance();
    static void SetInstance(MockStm32F4HalUart* instance);

private:
    static MockStm32F4HalUart* instance_;
};

#endif // MOCK_STM32F4_HAL_UART_HPP
