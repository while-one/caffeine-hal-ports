#include "mock_stm32f4_hal_uart.hpp"

MockStm32F4HalUart* MockStm32F4HalUart::instance_ = nullptr;

MockStm32F4HalUart* MockStm32F4HalUart::GetInstance() {
    return instance_;
}

void MockStm32F4HalUart::SetInstance(MockStm32F4HalUart* instance) {
    instance_ = instance;
}

extern "C" {

HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart) {
    if (MockStm32F4HalUart::GetInstance()) {
        return MockStm32F4HalUart::GetInstance()->HAL_UART_Init(huart);
    }
    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart) {
    if (MockStm32F4HalUart::GetInstance()) {
        return MockStm32F4HalUart::GetInstance()->HAL_UART_DeInit(huart);
    }
    return HAL_OK;
}

void HAL_UART_IRQHandler(UART_HandleTypeDef *huart) {
    if (MockStm32F4HalUart::GetInstance()) {
        MockStm32F4HalUart::GetInstance()->HAL_UART_IRQHandler(huart);
    }
}

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout) {
    if (MockStm32F4HalUart::GetInstance()) {
        return MockStm32F4HalUart::GetInstance()->HAL_UART_Transmit(huart, pData, Size, Timeout);
    }
    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout) {
    if (MockStm32F4HalUart::GetInstance()) {
        return MockStm32F4HalUart::GetInstance()->HAL_UART_Receive(huart, pData, Size, Timeout);
    }
    return HAL_OK;
}

HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {
    if (MockStm32F4HalUart::GetInstance()) {
        return MockStm32F4HalUart::GetInstance()->HAL_UARTEx_ReceiveToIdle_IT(huart, pData, Size);
    }
    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size) {
    if (MockStm32F4HalUart::GetInstance()) {
        return MockStm32F4HalUart::GetInstance()->HAL_UART_Transmit_IT(huart, pData, Size);
    }
    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_AbortTransmit_IT(UART_HandleTypeDef *huart) {
    if (MockStm32F4HalUart::GetInstance()) {
        return MockStm32F4HalUart::GetInstance()->HAL_UART_AbortTransmit_IT(huart);
    }
    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) {
    if (MockStm32F4HalUart::GetInstance()) {
        return MockStm32F4HalUart::GetInstance()->HAL_UART_Receive_IT(huart, pData, Size);
    }
    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_AbortReceive_IT(UART_HandleTypeDef *huart) {
    if (MockStm32F4HalUart::GetInstance()) {
        return MockStm32F4HalUart::GetInstance()->HAL_UART_AbortReceive_IT(huart);
    }
    return HAL_OK;
}

} // extern "C"
