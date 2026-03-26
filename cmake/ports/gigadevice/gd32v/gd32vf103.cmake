set(CAFFEINE_MCU_ARCH "rv32imac" CACHE STRING "MCU Architecture")
set(CAFFEINE_MCU_ABI "ilp32" CACHE STRING "MCU ABI")
set(CAFFEINE_LINKER_SCRIPT "GD32VF103_FLASH.ld" CACHE STRING "Linker script")

# Enable typical basic peripherals for gd32v, if any
set(CFN_HAL_CLOCK "ON" CACHE STRING "")
set(CFN_HAL_GPIO "ON" CACHE STRING "")
set(CFN_HAL_UART "ON" CACHE STRING "")
