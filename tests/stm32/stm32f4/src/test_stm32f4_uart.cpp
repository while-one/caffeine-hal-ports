#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mock_stm32f4_hal_uart.hpp"
#include "mock_stm32f4_hal_base.hpp"

extern "C"
{
#include "stm32f4xx_hal.h"
#include "cfn_hal_uart.h"
#include "cfn_hal_clock.h"
#include "cfn_hal_uart_port.h"
}

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SaveArg;

class MockHalUart : public MockStm32F4HalUart
{
  public:
    MOCK_METHOD(HAL_StatusTypeDef, HAL_UART_Init, (UART_HandleTypeDef * huart), (override));
    MOCK_METHOD(HAL_StatusTypeDef, HAL_UART_DeInit, (UART_HandleTypeDef * huart), (override));
    MOCK_METHOD(void, HAL_UART_IRQHandler, (UART_HandleTypeDef * huart), (override));
    MOCK_METHOD(HAL_StatusTypeDef,
                HAL_UART_Transmit,
                (UART_HandleTypeDef * huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout),
                (override));
    MOCK_METHOD(HAL_StatusTypeDef,
                HAL_UART_Receive,
                (UART_HandleTypeDef * huart, uint8_t *pData, uint16_t Size, uint32_t Timeout),
                (override));
    MOCK_METHOD(HAL_StatusTypeDef,
                HAL_UARTEx_ReceiveToIdle_IT,
                (UART_HandleTypeDef * huart, uint8_t *pData, uint16_t Size),
                (override));
    MOCK_METHOD(HAL_StatusTypeDef,
                HAL_UART_Transmit_IT,
                (UART_HandleTypeDef * huart, const uint8_t *pData, uint16_t Size),
                (override));
    MOCK_METHOD(HAL_StatusTypeDef, HAL_UART_AbortTransmit_IT, (UART_HandleTypeDef * huart), (override));
    MOCK_METHOD(HAL_StatusTypeDef,
                HAL_UART_Receive_IT,
                (UART_HandleTypeDef * huart, uint8_t *pData, uint16_t Size),
                (override));
    MOCK_METHOD(HAL_StatusTypeDef, HAL_UART_AbortReceive_IT, (UART_HandleTypeDef * huart), (override));
    MOCK_METHOD(HAL_StatusTypeDef,
                HAL_UART_Transmit_DMA,
                (UART_HandleTypeDef * huart, const uint8_t *pData, uint16_t Size),
                (override));
    MOCK_METHOD(HAL_StatusTypeDef,
                HAL_UART_Receive_DMA,
                (UART_HandleTypeDef * huart, uint8_t *pData, uint16_t Size),
                (override));
};

class STM32F4UartTest : public ::testing::Test
{
  protected:
    cfn_hal_uart_t        driver;
    cfn_hal_uart_config_t config;
    cfn_hal_uart_phy_t    phy;
    cfn_hal_clock_t       clock;
    MockHalUart           mock_hal;

    void SetUp() override
    {
        MockStm32F4HalUart::SetInstance(&mock_hal);

        // Dummy clock
        memset(&clock, 0, sizeof(clock));

        // Valid physical configuration for USART1
        phy.instance     = (void *) CFN_HAL_UART_PORT_USART1;
        phy.tx           = nullptr;
        phy.rx           = nullptr;

        // Valid standard configuration
        config.baudrate  = 115200;
        config.data_len  = CFN_HAL_UART_CONFIG_DATA_LEN_8;
        config.stop_bits = CFN_HAL_UART_CONFIG_STOP_ONE_BIT;
        config.parity    = CFN_HAL_UART_CONFIG_PARITY_NONE;
        config.direction = CFN_HAL_UART_CONFIG_DIRECTION_TX_RX;
        config.flow_ctrl = CFN_HAL_UART_CONFIG_FLOW_CTRL_NONE;
    }

    void TearDown() override
    {
        MockStm32F4HalUart::SetInstance(nullptr);
    }
};

TEST_F(STM32F4UartTest, HardwareMappingCorrectness)
{
    // Construct the driver
    cfn_hal_error_code_t err = cfn_hal_uart_construct(&driver, &config, &phy, &clock, nullptr, nullptr, nullptr);
    ASSERT_EQ(err, CFN_HAL_ERROR_OK);

    // Capture the pointer passed to HAL_UART_Init using GMock
    UART_HandleTypeDef *captured_handle = nullptr;
    EXPECT_CALL(mock_hal, HAL_UART_Init(_)).WillOnce(DoAll(SaveArg<0>(&captured_handle), Return(HAL_OK)));

    // Call init (this routes to port_base_init which calls HAL_UART_Init)
    err = cfn_hal_uart_init(&driver);
    ASSERT_EQ(err, CFN_HAL_ERROR_OK);

    // Verify HAL_UART_Init was called and captured the handle
    ASSERT_NE(captured_handle, nullptr);

    // Verify mapping from Caffeine types to STM32 types
    EXPECT_EQ(captured_handle->Init.BaudRate, 115200);
    EXPECT_EQ(captured_handle->Init.WordLength, UART_WORDLENGTH_8B);
    EXPECT_EQ(captured_handle->Init.StopBits, UART_STOPBITS_1);
    EXPECT_EQ(captured_handle->Init.Parity, UART_PARITY_NONE);
    EXPECT_EQ(captured_handle->Init.Mode, UART_MODE_TX_RX);
    EXPECT_EQ(captured_handle->Init.HwFlowCtl, UART_HWCONTROL_NONE);

    // Cleanup
    EXPECT_CALL(mock_hal, HAL_UART_DeInit(_)).WillOnce(Return(HAL_OK));
    cfn_hal_uart_deinit(&driver);
    cfn_hal_uart_destruct(&driver);
}

TEST_F(STM32F4UartTest, HalInitFailurePropagatesError)
{
    cfn_hal_uart_construct(&driver, &config, &phy, &clock, nullptr, nullptr, nullptr);

    // Force the mock HAL to return an error
    EXPECT_CALL(mock_hal, HAL_UART_Init(_)).WillOnce(Return(HAL_ERROR));

    cfn_hal_error_code_t err = cfn_hal_uart_init(&driver);

    // cfn_hal_stm32_map_error should map HAL_ERROR to CFN_HAL_ERROR_FAIL
    EXPECT_EQ(err, CFN_HAL_ERROR_FAIL);
}
