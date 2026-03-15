add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gigadevice/gd32v/cfn_hal_uart.c
)

# Propagate RISC-V architecture and ABI flags
target_compile_options(${PROJECT_NAME} PUBLIC
    -march=${CAFFEINE_MCU_ARCH}
    -mabi=${CAFFEINE_MCU_ABI}
    -mcmodel=medlow
)

# Propagate any additional MCU-specific compilation flags
if(DEFINED CAFFEINE_MCU_COMPILE_OPTIONS)
    separate_arguments(EXTRA_FLAGS_LIST NATIVE_COMMAND ${CAFFEINE_MCU_COMPILE_OPTIONS})
    target_compile_options(${PROJECT_NAME} PUBLIC ${EXTRA_FLAGS_LIST})
endif()

# Bootloader Support: Relocate Vector Table (mtvec CSR)
if(DEFINED CAFFEINE_BOOTLOADER_SIZE_HEX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        USER_VECT_TAB_ADDRESS
        VECT_TAB_OFFSET=${CAFFEINE_BOOTLOADER_SIZE_HEX}
    )
endif()

# The generated linker script path (pending script implementation)
set(GENERATED_LINKER_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/generated_${CAFFEINE_BOARD_LINKER}")

# Propagate Linker Script
target_link_options(${PROJECT_NAME} INTERFACE
    -T ${CMAKE_CURRENT_LIST_DIR}/../../../linker/${CAFFEINE_BOARD_LINKER}
    -Wl,--gc-sections
    -nostartfiles
)
