# --- Modular Source Collection Logic ---
set(PORT_SOURCES_LIST 
    "${PROJECT_SOURCE_DIR}/src/gigadevice/gd32v/cfn_hal_uart.c"
)

# --- Main Port Target Configuration (delegated to root CMakeLists) ---
macro(cfn_port_apply_target_config TARGET_NAME)
    # Link against the generic HAL interface
    target_link_libraries(${TARGET_NAME} PUBLIC caffeine::hal)

    # Propagate RISC-V architecture and ABI flags
    target_compile_options(${TARGET_NAME} PRIVATE
        -march=${CAFFEINE_MCU_ARCH}
        -mabi=${CAFFEINE_MCU_ABI}
        -mcmodel=medlow
    )
    target_compile_options(${TARGET_NAME} INTERFACE
        -march=${CAFFEINE_MCU_ARCH}
        -mabi=${CAFFEINE_MCU_ABI}
        -mcmodel=medlow
    )

    # Propagate any additional MCU-specific compilation flags
    if(DEFINED CAFFEINE_MCU_COMPILE_OPTIONS)
        separate_arguments(EXTRA_FLAGS_LIST NATIVE_COMMAND ${CAFFEINE_MCU_COMPILE_OPTIONS})
        target_compile_options(${TARGET_NAME} PRIVATE ${EXTRA_FLAGS_LIST})
        target_compile_options(${TARGET_NAME} INTERFACE ${EXTRA_FLAGS_LIST})
    endif()

    # Bootloader Support: Relocate Vector Table (mtvec CSR)
    if(DEFINED CAFFEINE_BOOTLOADER_SIZE_HEX)
        target_compile_definitions(${TARGET_NAME} PUBLIC
            USER_VECT_TAB_ADDRESS
            VECT_TAB_OFFSET=${CAFFEINE_BOOTLOADER_SIZE_HEX}
        )
    endif()

    # Propagate Linker Script
    if(DEFINED CAFFEINE_BOARD_LINKER)
        target_link_options(${TARGET_NAME} INTERFACE
            -T ${PROJECT_SOURCE_DIR}/linker/${CAFFEINE_BOARD_LINKER}
            -Wl,--gc-sections
            -nostartfiles
        )
    endif()

    target_include_directories(${TARGET_NAME} PUBLIC 
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/gigadevice/gd32v>
    )
endmacro()
