#ifndef MOCK_STM32F4_DEFINES_H
#define MOCK_STM32F4_DEFINES_H

/* Exhaustive Peripheral Instances for 100% Port Compilation Coverage */

// UART/USART
#define USART1 ((USART_TypeDef *) 0x40011000)
#define USART2 ((USART_TypeDef *) 0x40004400)
#define USART3 ((USART_TypeDef *) 0x40004800)
#define UART4  ((USART_TypeDef *) 0x40004C00)
#define UART5  ((USART_TypeDef *) 0x40005000)
#define USART6 ((USART_TypeDef *) 0x40011400)
#define UART7  ((USART_TypeDef *) 0x40007800)
#define UART8  ((USART_TypeDef *) 0x40007C00)
#define UART9  ((USART_TypeDef *) 0x40011800)
#define UART10 ((USART_TypeDef *) 0x40011C00)

// SPI
#define SPI1 ((SPI_TypeDef *) 0x40013000)
#define SPI2 ((SPI_TypeDef *) 0x40003800)
#define SPI3 ((SPI_TypeDef *) 0x40003C00)
#define SPI4 ((SPI_TypeDef *) 0x40013400)
#define SPI5 ((SPI_TypeDef *) 0x40015000)
#define SPI6 ((SPI_TypeDef *) 0x40015400)

// I2C
#define I2C1 ((I2C_TypeDef *) 0x40005400)
#define I2C2 ((I2C_TypeDef *) 0x40005800)
#define I2C3 ((I2C_TypeDef *) 0x40005C00)
#define I2C4 ((I2C_TypeDef *) 0x40006000)

// ADC
#define ADC1 ((ADC_TypeDef *) 0x40012000)
#define ADC2 ((ADC_TypeDef *) 0x40012100)
#define ADC3 ((ADC_TypeDef *) 0x40012200)

// CAN
#define CAN1 ((CAN_TypeDef *) 0x40006400)
#define CAN2 ((CAN_TypeDef *) 0x40006800)
#define CAN3 ((CAN_TypeDef *) 0x40006C00)

// TIM
#define TIM1  ((TIM_TypeDef *) 0x40010000)
#define TIM2  ((TIM_TypeDef *) 0x40000000)
#define TIM3  ((TIM_TypeDef *) 0x40000400)
#define TIM4  ((TIM_TypeDef *) 0x40000800)
#define TIM5  ((TIM_TypeDef *) 0x40000C00)
#define TIM6  ((TIM_TypeDef *) 0x40001000)
#define TIM7  ((TIM_TypeDef *) 0x40001400)
#define TIM8  ((TIM_TypeDef *) 0x40010400)
#define TIM9  ((TIM_TypeDef *) 0x40014000)
#define TIM10 ((TIM_TypeDef *) 0x40014400)
#define TIM11 ((TIM_TypeDef *) 0x40014800)
#define TIM12 ((TIM_TypeDef *) 0x40001800)
#define TIM13 ((TIM_TypeDef *) 0x40001C00)
#define TIM14 ((TIM_TypeDef *) 0x40002000)

/* RCC Enable Macros for all possible peripherals */
/* We use unique dummy variables to ensure clang-tidy doesn't flag identical branches */
static volatile uint32_t __mock_rcc_gate;
#define __HAL_RCC_UART7_CLK_ENABLE()  do { __mock_rcc_gate = 7; } while(0)
#define __HAL_RCC_UART8_CLK_ENABLE()  do { __mock_rcc_gate = 8; } while(0)
#define __HAL_RCC_UART9_CLK_ENABLE()  do { __mock_rcc_gate = 9; } while(0)
#define __HAL_RCC_UART10_CLK_ENABLE() do { __mock_rcc_gate = 10; } while(0)
#define __HAL_RCC_SPI4_CLK_ENABLE()   do { __mock_rcc_gate = 44; } while(0)
#define __HAL_RCC_SPI5_CLK_ENABLE()   do { __mock_rcc_gate = 55; } while(0)
#define __HAL_RCC_SPI6_CLK_ENABLE()   do { __mock_rcc_gate = 66; } while(0)
#define __HAL_RCC_CAN3_CLK_ENABLE()   do { __mock_rcc_gate = 333; } while(0)

/* Missing register defines that some headers might expect */
#define RCC_APB2ENR_SPI4EN 0
#define RCC_APB2ENR_SPI5EN 0
#define RCC_APB2ENR_SPI6EN 0

#endif // MOCK_STM32F4_DEFINES_H
