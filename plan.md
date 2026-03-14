# STM32F4 Port Assessment and Implementation Plan

## 1. Executive Summary & Architectural Assessment

The current state of the STM32F4 port demonstrates a solid architectural foundation. It correctly adheres to the Caffeine-HAL design principles:
*   **Virtual Method Tables (VMTs)** are properly utilized to map generic APIs to vendor functions.
*   **Decoupling:** Hardware handles (`UART_HandleTypeDef`, etc.) are hidden within the port `.c` files using static arrays indexed by the user's opaque instance ID in $O(1)$ time.
*   **Error Handling:** The port uses a centralized `cfn_hal_stm32_map_error()` macro to correctly translate ST HAL statuses into `cfn_hal_error_code_t`.
*   **Clock Gating:** A centralized `cfn_hal_port_clock_enable_gate` is implemented elegantly.

### ⚠️ Critical Violations of the API Contract
While basic polling operations are implemented, the port currently fakes its event management.
In almost every file, functions like `event_enable` and `event_disable` simply return `CFN_HAL_ERROR_OK` **without modifying any hardware registers** (e.g., `__HAL_UART_ENABLE_IT`). 
This is a critical flaw: applications will assume interrupts are enabled when they are physically disabled, leading to silent failures in asynchronous designs.

*(Note: `callback_register` returning `OK` is acceptable, as the actual callback pointer storage is handled by the inline wrappers in the core `caffeine-hal` repository.)*

---

## 2. Missing Implementations Grouped by File

### A. Fully Stubbed Peripherals
The following files are essentially empty templates returning `CFN_HAL_ERROR_NOT_SUPPORTED` for all API extensions. They require 100% implementation from scratch:
*   `cfn_hal_comp_port.c` (Comparators)
*   `cfn_hal_crypto_port.c` (Hardware Cryptography)
*   `cfn_hal_dma_port.c` (Direct Memory Access)
*   `cfn_hal_i2s_port.c` (Inter-IC Sound)
*   `cfn_hal_irq_port.c` (NVIC/Interrupt Management)
*   `cfn_hal_nvm_port.c` (Internal Flash Memory)
*   `cfn_hal_pwm_port.c` (Pulse Width Modulation)
*   `cfn_hal_qspi_port.c` (Quad-SPI)
*   `cfn_hal_sdio_port.c` (SD/MMC Interface)

### B. Partially Implemented Peripherals
The following files have basic polling and initialization implemented, but lack asynchronous operations or advanced features.

**1. `cfn_hal_uart_port.c`**
*   `port_uart_tx_irq`, `port_uart_tx_irq_abort`
*   `port_uart_rx_irq`, `port_uart_rx_irq_abort`
*   `port_uart_rx_to_idle`
*   `port_uart_tx_dma`, `port_uart_rx_dma`

**2. `cfn_hal_spi_port.c`**
*   `port_spi_xfr_irq`, `port_spi_xfr_irq_abort`
*   `port_spi_xfr_dma`

**3. `cfn_hal_i2c_port.c`**
*   `port_i2c_xfr_irq`, `port_i2c_xfr_irq_abort`
*   `port_i2c_xfr_dma`

**4. `cfn_hal_adc_port.c`**
*   `port_adc_start`
*   `port_adc_stop`
*   `port_adc_read_dma`

**5. `cfn_hal_dac_port.c`**
*   `port_dac_write_dma`

**6. `cfn_hal_eth_port.c`**
*   `port_eth_receive_frame` (Missing RX processing)
*   `port_eth_read_phy_reg`, `port_eth_write_phy_reg` (Missing MDIO configuration)
*   `port_eth_get_link_status` (Missing PHY status reading)

**7. `cfn_hal_rtc_port.c`**
*   `port_rtc_set_alarm`
*   `port_rtc_get_alarm`
*   `port_rtc_stop_alarm`

**8. `cfn_hal_clock_port.c`**
*   `port_clock_disable_gate`

### C. Base Driver Violations (Across All Files)
The following generic VMT functions are implemented as dummy stubs returning `CFN_HAL_ERROR_OK` in `UART`, `SPI`, `I2C`, `ADC`, `DAC`, `CAN`, `TIMER`, `ETH`, `RTC`, `WDT`:
*   `event_enable` / `event_disable` / `event_get`
*   `error_enable` / `error_disable` / `error_get`
*   `callback_register`

---

## 3. Implementation Plan (Phases)

To bring the STM32F4 port up to production standard, work should proceed in the following prioritized phases:

### Phase 1: Event & Interrupt Infrastructure
*Before asynchronous functions can work, the interrupt pipeline must be fixed.*
1.  Implement `cfn_hal_irq_port.c` to wrap NVIC operations.
2.  Implement `event_enable` and `error_enable` using vendor macros (e.g., `__HAL_UART_ENABLE_IT(huart, UART_IT_RXNE)`).
3.  Write the actual global ISR handlers (e.g., `void USART1_IRQHandler(void)`) to map hardware flags back to the user's `cb` function with the correct event masks.

### Phase 2: Asynchronous Communication
1.  Implement `cfn_hal_dma_port.c` to provide stream/channel allocation.
2.  Implement the `_irq` and `_irq_abort` methods for `UART`, `SPI`, and `I2C`.
3.  Implement the `_dma` methods for `UART`, `SPI`, `I2C`, `ADC`, and `DAC`.

### Phase 3: High-Priority Incomplete Peripherals
1.  **ETH:** Complete the `receive_frame` logic using the ST HAL descriptor rings. Implement MDIO interactions for PHY reads/writes.
2.  **RTC:** Implement the Alarm A/B functionalities.
3.  **UART:** Implement the highly useful `rx_to_idle` function using UART Idle Line detection.

### Phase 4: Implementation of Stubbed Peripherals
Develop the completely missing ports based on application requirements. Suggested order:
1.  `cfn_hal_pwm_port.c` (Timer advanced features)
2.  `cfn_hal_qspi_port.c` / `cfn_hal_sdio_port.c` (External memory)
3.  `cfn_hal_nvm_port.c` (Internal Flash manipulation)
4.  `cfn_hal_crypto_port.c` (Hardware crypto acceleration)
5.  `cfn_hal_i2s_port.c` & `cfn_hal_comp_port.c`
