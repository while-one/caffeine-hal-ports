include(FetchContent)

# Fetch the Vendor SDK dynamically. We avoid git submodules and keep the repo lightweight.
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

if(NOT DEFINED CAFFEINE_MCU_MACRO)
    message(FATAL_ERROR "CAFFEINE_MCU_MACRO is required to build the STM32F4 port (e.g., STM32F407xx). Please define it in your CMakePreset.")
endif()

# --- Dynamic Assembly Startup Resolution ---
# The user provides CAFFEINE_MCU_MACRO (e.g., STM32F417xx)
# We must convert this to lowercase (stm32f417xx) to find the correct .s file
string(TOLOWER "${CAFFEINE_MCU_MACRO}" LOWERCASE_MCU_MACRO)
set(STARTUP_FILE "${SDK_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_${LOWERCASE_MCU_MACRO}.s")

# --- 1. Vendor SDK Isolation Target ---
# We create a separate OBJECT target for the vendor code to relax its compilation flags.
# An OBJECT library compiles to .o files without creating an archive, allowing us to
# merge it cleanly into the main port library later for perfect encapsulation.
add_library(vendor_sdk OBJECT
    # Vendor SDK System & Core
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    ${SDK_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
    
    # Vendor SDK Peripherals
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_usart.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c

    # Silicon-Specific Assembly Startup
    ${STARTUP_FILE}
)

# Relax compilation flags for vendor code
target_compile_options(vendor_sdk PRIVATE -w)

# Include Vendor SDK headers AND our local directory containing stm32f4xx_hal_conf.h
target_include_directories(vendor_sdk SYSTEM PRIVATE
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${SDK_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${SDK_DIR}/Drivers/CMSIS/Include
)
target_include_directories(vendor_sdk PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stm32/stm32f4
)

# Propagate Vendor Macros needed by the SDK
target_compile_definitions(vendor_sdk PUBLIC
    USE_HAL_DRIVER
    ${CAFFEINE_MCU_MACRO}
)

# Propagate Silicon-specific CPU architecture flags
set(CPU_FLAGS -mcpu=${CAFFEINE_MCU_CORE} -mthumb)
target_compile_options(vendor_sdk PUBLIC ${CPU_FLAGS})

# --- 2. Main Port Library ---
# Gather all Caffeine VMT Wrappers
file(GLOB PORT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/stm32/stm32f4/cfn_hal_*_port.c")

add_library(${PROJECT_NAME} STATIC
    ${PORT_SOURCES}
)

# Apply CPU flags to the main library as well
target_compile_options(${PROJECT_NAME} PUBLIC ${CPU_FLAGS})

# Absorb the isolated vendor SDK object files into our main port library
target_link_libraries(${PROJECT_NAME} PRIVATE $<TARGET_OBJECTS:vendor_sdk>)

# Include directories needed by our VMT wrappers
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${SDK_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${SDK_DIR}/Drivers/CMSIS/Include
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stm32/stm32f4
)

# Propagate Vendor Macros to the main target
target_compile_definitions(${PROJECT_NAME} PUBLIC
    USE_HAL_DRIVER
    ${CAFFEINE_MCU_MACRO}
)

# Propagate any additional MCU-specific compilation flags
if(DEFINED CAFFEINE_MCU_COMPILE_OPTIONS)
    separate_arguments(EXTRA_FLAGS_LIST NATIVE_COMMAND ${CAFFEINE_MCU_COMPILE_OPTIONS})
    target_compile_options(${PROJECT_NAME} PUBLIC ${EXTRA_FLAGS_LIST})
endif()

# Bootloader Support: Relocate Vector Table (VTOR)
if(DEFINED CAFFEINE_BOOTLOADER_SIZE_HEX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        USER_VECT_TAB_ADDRESS
        VECT_TAB_OFFSET=${CAFFEINE_BOOTLOADER_SIZE_HEX}
    )
endif()

# The generated linker script path (pending script implementation)
set(GENERATED_LINKER_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/generated_${CAFFEINE_BOARD_LINKER}")

# Propagate Board-specific Linker Script and generate a memory map
target_link_options(${PROJECT_NAME} INTERFACE
    -T ${GENERATED_LINKER_SCRIPT}
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/firmware.map
)
