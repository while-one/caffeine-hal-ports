#ifndef MOCK_STM32F4_HAL_BASE_HPP
#define MOCK_STM32F4_HAL_BASE_HPP

#include <gmock/gmock.h>

extern "C" {
#include "stm32f4xx_hal.h"
}

class MockStm32F4HalBase {
public:
    virtual ~MockStm32F4HalBase() = default;

    virtual HAL_StatusTypeDef HAL_Init(void) = 0;

    static MockStm32F4HalBase* GetInstance();
    static void SetInstance(MockStm32F4HalBase* instance);

private:
    static MockStm32F4HalBase* instance_;
};

#endif // MOCK_STM32F4_HAL_BASE_HPP
