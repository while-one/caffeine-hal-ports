include(FetchContent)

# Fetch the Vendor SDK dynamically. We avoid git submodules and keep the repo lightweight.
FetchContent_Declare(
    vendor_stm32cubeh5
    GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeH5.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(vendor_stm32cubeh5)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stm32/stm32h5/cfn_hal_uart.c
)

# Include Vendor SDK headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${vendor_stm32cubeh5_SOURCE_DIR}/Drivers/STM32H5xx_HAL_Driver/Inc
    ${vendor_stm32cubeh5_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32H5xx/Include
    ${vendor_stm32cubeh5_SOURCE_DIR}/Drivers/CMSIS/Include
)

# Propagate Vendor Macros needed by the SDK
target_compile_definitions(${PROJECT_NAME} PUBLIC
    USE_HAL_DRIVER
    ${CAFFEINE_MCU_MACRO}
)

# Bootloader Support: Relocate Vector Table (VTOR)
if(DEFINED CAFFEINE_BOOTLOADER_SIZE_HEX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        USER_VECT_TAB_ADDRESS
        VECT_TAB_OFFSET=${CAFFEINE_BOOTLOADER_SIZE_HEX}
    )
endif()

# Propagate Silicon-specific CPU architecture flags
target_compile_options(${PROJECT_NAME} PUBLIC
    -mcpu=${CAFFEINE_MCU_CORE}
    -mthumb
)

# Propagate any additional MCU-specific compilation flags (e.g., FPU, DSP extensions)
if(DEFINED CAFFEINE_MCU_COMPILE_OPTIONS)
    # We use separate_arguments to ensure CMake treats it as two flags instead of a quoted string
    separate_arguments(EXTRA_FLAGS_LIST NATIVE_COMMAND ${CAFFEINE_MCU_COMPILE_OPTIONS})
    target_compile_options(${PROJECT_NAME} PUBLIC ${EXTRA_FLAGS_LIST})
endif()

# The generated linker script path (pending script implementation)
set(GENERATED_LINKER_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/generated_${CAFFEINE_BOARD_LINKER}")

# Propagate Board-specific Linker Script
target_link_options(${PROJECT_NAME} INTERFACE
    -T ${CMAKE_CURRENT_LIST_DIR}/../../../linker/${CAFFEINE_BOARD_LINKER}
)
