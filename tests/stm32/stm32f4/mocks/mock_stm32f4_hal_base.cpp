#include "mock_stm32f4_hal_base.hpp"

MockStm32F4HalBase *MockStm32F4HalBase::instance_ = nullptr;

MockStm32F4HalBase *MockStm32F4HalBase::GetInstance()
{
    return instance_;
}

void MockStm32F4HalBase::SetInstance(MockStm32F4HalBase *instance)
{
    instance_ = instance;
}

extern "C"
{
HAL_StatusTypeDef HAL_Init(void)
{
    if (MockStm32F4HalBase::GetInstance())
    {
        return MockStm32F4HalBase::GetInstance()->HAL_Init();
    }
    return HAL_OK;
}
}
