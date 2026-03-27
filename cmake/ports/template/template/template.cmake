# This script defines the implementation-agnostic template port.
# It is used primarily for compliance analysis and architectural validation.

# Manually list the boilerplate files to be analyzed
set(PORT_SOURCES_LIST
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_adc_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_can_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_clock_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_comp_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_crypto_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_dac_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_dma_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_eth_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_gpio_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_i2c_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_i2s_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_irq_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_nvm_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_pwm_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_qspi_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_rtc_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_sdio_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_spi_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_timer_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_uart_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_usb_port.c
    ${PROJECT_SOURCE_DIR}/src/template/template/cfn_hal_wdt_port.c
)

# Main target configuration is handled by the root CMakeLists.txt
# using the PORT_SOURCES_LIST variable populated here.
