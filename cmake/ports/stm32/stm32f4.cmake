include(FetchContent)

# Fetch the Vendor SDK dynamically.
FetchContent_Declare(
    vendor_stm32cubef4
    GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeF4.git
    GIT_TAG        v1.28.3
    GIT_SUBMODULES 
        "Drivers/STM32F4xx_HAL_Driver"
        "Drivers/CMSIS/Device/ST/STM32F4xx"
)
FetchContent_MakeAvailable(vendor_stm32cubef4)

set(SDK_DIR ${vendor_stm32cubef4_SOURCE_DIR})
set(HAL_SRC_DIR "${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src")

if(NOT DEFINED CAFFEINE_MCU_MACRO)
    message(FATAL_ERROR "CAFFEINE_MCU_MACRO is required to build the STM32F4 port (e.g., STM32F407xx). Please define it in your CMakePreset.")
endif()

# --- Dynamic Assembly Startup Resolution ---
string(TOLOWER "${CAFFEINE_MCU_MACRO}" LOWERCASE_MCU_MACRO)
set(STARTUP_FILE "${SDK_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_${LOWERCASE_MCU_MACRO}.s")

# --- Modular Source Collection Logic ---
set(VENDOR_SOURCES_LIST "")
set(PORT_SOURCES_LIST "")

# Helper to conditionally add modules
macro(cfn_add_stm32f4_module NAME VENDOR_FILES PORT_FILES)
    # Port files are ALWAYS added to ensure construct/destruct symbols exist
    foreach(PFILE IN ITEMS ${PORT_FILES})
        list(APPEND PORT_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/${PFILE}")
    endforeach()

    if(CFN_HAL_${NAME} STREQUAL "ON")
        foreach(VFILE IN ITEMS ${VENDOR_FILES})
            list(APPEND VENDOR_SOURCES_LIST "${HAL_SRC_DIR}/${VFILE}")
        endforeach()
        set(HAL_${NAME}_MODULE_ENABLED 1)
    else()
        set(HAL_${NAME}_MODULE_ENABLED 0)
    endif()
endmacro()

# Always enable foundational modules
list(APPEND VENDOR_SOURCES_LIST 
    "${HAL_SRC_DIR}/stm32f4xx_hal.c"
    "${HAL_SRC_DIR}/stm32f4xx_hal_cortex.c"
    "${HAL_SRC_DIR}/stm32f4xx_hal_rcc.c"
    "${HAL_SRC_DIR}/stm32f4xx_hal_rcc_ex.c"
    "${HAL_SRC_DIR}/stm32f4xx_hal_pwr.c"
    "${HAL_SRC_DIR}/stm32f4xx_hal_pwr_ex.c"
    "${HAL_SRC_DIR}/stm32f4xx_hal_flash.c"
    "${HAL_SRC_DIR}/stm32f4xx_hal_flash_ex.c"
    "${SDK_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c"
)

# Map CFN_HAL flags to Vendor and Port files
cfn_add_stm32f4_module(ADC    "stm32f4xx_hal_adc.c;stm32f4xx_hal_adc_ex.c" "src/stm32/stm32f4/cfn_hal_adc_port.c")
cfn_add_stm32f4_module(CAN    "stm32f4xx_hal_can.c"                        "src/stm32/stm32f4/cfn_hal_can_port.c")
cfn_add_stm32f4_module(CLOCK  ""                                           "src/stm32/stm32f4/cfn_hal_clock_port.c")
cfn_add_stm32f4_module(COMP   "stm32f4xx_hal_comp.c"                       "src/stm32/stm32f4/cfn_hal_comp_port.c")
cfn_add_stm32f4_module(CRYPTO "stm32f4xx_hal_cryp.c;stm32f4xx_hal_cryp_ex.c" "src/stm32/stm32f4/cfn_hal_crypto_port.c")
cfn_add_stm32f4_module(DAC    "stm32f4xx_hal_dac.c;stm32f4xx_hal_dac_ex.c" "src/stm32/stm32f4/cfn_hal_dac_port.c")
cfn_add_stm32f4_module(DMA    "stm32f4xx_hal_dma.c;stm32f4xx_hal_dma_ex.c" "src/stm32/stm32f4/cfn_hal_dma_port.c")
cfn_add_stm32f4_module(ETH    "stm32f4xx_hal_eth.c"                        "src/stm32/stm32f4/cfn_hal_eth_port.c")
cfn_add_stm32f4_module(GPIO   "stm32f4xx_hal_gpio.c;stm32f4xx_hal_exti.c"  "src/stm32/stm32f4/cfn_hal_gpio_port.c")
cfn_add_stm32f4_module(I2C    "stm32f4xx_hal_i2c.c;stm32f4xx_hal_i2c_ex.c" "src/stm32/stm32f4/cfn_hal_i2c_port.c")
cfn_add_stm32f4_module(I2S    "stm32f4xx_hal_i2s.c;stm32f4xx_hal_i2s_ex.c" "src/stm32/stm32f4/cfn_hal_i2s_port.c")
cfn_add_stm32f4_module(IRQ    ""                                           "src/stm32/stm32f4/cfn_hal_irq_port.c")
cfn_add_stm32f4_module(NVM    ""                                           "src/stm32/stm32f4/cfn_hal_nvm_port.c")
cfn_add_stm32f4_module(PWM    "stm32f4xx_hal_tim.c;stm32f4xx_hal_tim_ex.c" "src/stm32/stm32f4/cfn_hal_pwm_port.c")
cfn_add_stm32f4_module(QSPI   "stm32f4xx_hal_qspi.c"                       "src/stm32/stm32f4/cfn_hal_qspi_port.c")
cfn_add_stm32f4_module(RTC    "stm32f4xx_hal_rtc.c;stm32f4xx_hal_rtc_ex.c" "src/stm32/stm32f4/cfn_hal_rtc_port.c")
cfn_add_stm32f4_module(SDIO   "stm32f4xx_hal_sd.c;stm32f4xx_ll_sdmmc.c"    "src/stm32/stm32f4/cfn_hal_sdio_port.c")
cfn_add_stm32f4_module(SPI    "stm32f4xx_hal_spi.c"                        "src/stm32/stm32f4/cfn_hal_spi_port.c")
cfn_add_stm32f4_module(TIMER  "stm32f4xx_hal_tim.c;stm32f4xx_hal_tim_ex.c" "src/stm32/stm32f4/cfn_hal_timer_port.c")
cfn_add_stm32f4_module(UART   "stm32f4xx_hal_uart.c;stm32f4xx_hal_usart.c" "src/stm32/stm32f4/cfn_hal_uart_port.c")
cfn_add_stm32f4_module(USB    "stm32f4xx_hal_pcd.c;stm32f4xx_hal_pcd_ex.c;stm32f4xx_ll_usb.c" "src/stm32/stm32f4/cfn_hal_usb_port.c")
cfn_add_stm32f4_module(WDT    "stm32f4xx_hal_iwdg.c;stm32f4xx_hal_wwdg.c"  "src/stm32/stm32f4/cfn_hal_wdt_port.c")

# Special handling for sub-features of CRYPTO
if(CFN_HAL_HASH STREQUAL "ON")
    list(APPEND VENDOR_SOURCES_LIST "${HAL_SRC_DIR}/stm32f4xx_hal_hash.c" "${HAL_SRC_DIR}/stm32f4xx_hal_hash_ex.c")
    set(HAL_HASH_MODULE_ENABLED 1)
endif()
if(CFN_HAL_RNG STREQUAL "ON")
    list(APPEND VENDOR_SOURCES_LIST "${HAL_SRC_DIR}/stm32f4xx_hal_rng.c")
    set(HAL_RNG_MODULE_ENABLED 1)
endif()

# --- Generate stm32f4xx_hal_conf.h ---
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/stm32/stm32f4/stm32f4xx_hal_conf.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/stm32f4xx_hal_conf.h"
)

# --- 1. Vendor SDK Isolation Target ---
add_library(vendor_sdk OBJECT ${VENDOR_SOURCES_LIST} ${STARTUP_FILE})
target_compile_options(vendor_sdk PRIVATE -w)
target_link_libraries(vendor_sdk PRIVATE caffeine::hal)

set(CPU_FLAGS -mcpu=${CAFFEINE_MCU_CORE} -mthumb)
target_compile_options(vendor_sdk PUBLIC ${CPU_FLAGS})
target_compile_options(vendor_sdk PRIVATE ${CPU_FLAGS})

if(DEFINED CAFFEINE_MCU_COMPILE_OPTIONS)
    separate_arguments(MCU_FLAGS_LIST NATIVE_COMMAND ${CAFFEINE_MCU_COMPILE_OPTIONS})
    target_compile_options(vendor_sdk PUBLIC ${MCU_FLAGS_LIST})
    target_compile_options(vendor_sdk PRIVATE ${MCU_FLAGS_LIST})
endif()

target_include_directories(vendor_sdk SYSTEM PRIVATE
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${SDK_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${SDK_DIR}/Drivers/CMSIS/Include
)
target_include_directories(vendor_sdk PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)

target_compile_definitions(vendor_sdk PUBLIC USE_HAL_DRIVER ${CAFFEINE_MCU_MACRO})

# --- 2. Main Port Library ---
add_library(${PROJECT_NAME} STATIC ${PORT_SOURCES_LIST})
target_compile_options(${PROJECT_NAME} PRIVATE ${CPU_FLAGS})
target_compile_options(${PROJECT_NAME} INTERFACE ${CPU_FLAGS})
target_link_options(${PROJECT_NAME} INTERFACE ${CPU_FLAGS})

if(DEFINED CAFFEINE_MCU_COMPILE_OPTIONS)
    separate_arguments(MCU_FLAGS_LIST NATIVE_COMMAND ${CAFFEINE_MCU_COMPILE_OPTIONS})
    target_compile_options(${PROJECT_NAME} PRIVATE ${MCU_FLAGS_LIST})
    target_compile_options(${PROJECT_NAME} INTERFACE ${MCU_FLAGS_LIST})
    target_link_options(${PROJECT_NAME} INTERFACE ${MCU_FLAGS_LIST})
endif()

# We link vendor_sdk INTERFACE so that downstream executables pull in the object files (startup, etc)
target_link_libraries(${PROJECT_NAME} INTERFACE $<TARGET_OBJECTS:vendor_sdk>)
target_link_libraries(${PROJECT_NAME} PRIVATE $<TARGET_OBJECTS:vendor_sdk>)

# Export port-specific headers and generated config to downstream users
target_include_directories(${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../../src/stm32/stm32f4>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${SDK_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${SDK_DIR}/Drivers/CMSIS/Include
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)

target_compile_definitions(${PROJECT_NAME} PUBLIC USE_HAL_DRIVER ${CAFFEINE_MCU_MACRO})

if(DEFINED CAFFEINE_BOOTLOADER_SIZE_HEX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC USER_VECT_TAB_ADDRESS VECT_TAB_OFFSET=${CAFFEINE_BOOTLOADER_SIZE_HEX})
endif()

if(DEFINED CAFFEINE_LINKER_SCRIPT)
    find_file(LINKER_SCRIPT_FULL_PATH 
        NAMES ${CAFFEINE_LINKER_SCRIPT}
        PATHS "${CMAKE_CURRENT_LIST_DIR}/../../../linker"
        NO_DEFAULT_PATH
    )
    if(LINKER_SCRIPT_FULL_PATH)
        target_link_options(${PROJECT_NAME} INTERFACE
            -T "${LINKER_SCRIPT_FULL_PATH}"
        )
    endif()
endif()

