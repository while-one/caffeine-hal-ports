# STM32 Porting Guide - Agent Skill Definition

<instructions>
You are an expert embedded C developer and CMake architect. Your task is to implement a new STM32 microcontroller family port (e.g., STM32H7, STM32L4) into the `caffeine-hal-ports` repository.

Before you modify any CMake recipes or write any C code, you MUST follow this sequence:
1. **Identify Target:** Ensure you know the target MCU family and specific MCU (e.g., from the user's initial prompt). If not provided, ask for it first.
2. **Pre-flight Check:** Inspect the repository to determine the current state of the requested port. Scaffolding might already exist:
   - Check `CMakePresets.json` for existing presets for this specific MCU.
   - Check if `cmake/ports/stm32/<family>.cmake` exists and if it actually contains `FetchContent` logic or just placeholders.
   - Check if `src/stm32/cfn_hal_stm32_error.h` exists (it should).
   - Check `src/stm32/<family>/` for an existing `hal_conf.h` and check if VMT wrapper C files are implemented or missing.
   - **CRITICAL:** If the pre-flight check reveals that the specific MCU is already fully supported and ported, STOP. Inform the user that the port already exists and no further action is required.
3. **Gather Missing Info:** If the port is missing or only partially scaffolded, ask the user ONLY for the prerequisite information (listed in Section 1) that is still missing.
4. **Execute:** Adapt the Architectural Execution Plan (Section 2) to execute ONLY the steps that are incomplete. Do not blindly overwrite existing, valid work.
</instructions>

## 1. Prerequisite Information Gathering
Once the target family is known and the pre-flight check is complete, stop and ask the user for any of the following details that are still required:

1.  **Target Family Name:** (e.g., `stm32h7`, `stm32g4`). This defines the `CAFFEINE_PORT_FAMILY` and directory names.
2.  **Vendor SDK GitHub Repository:** The official STMicroelectronics GitHub URL for the MCU family (e.g., `https://github.com/STMicroelectronics/STM32CubeH7.git`).
3.  **Vendor SDK Git Tag:** The specific version or tag of the SDK to lock the build to (e.g., `v1.11.0` or `main`).
4.  **Target MCU Macro (for testing/preset):** A specific silicon variant to use for generating a test preset (e.g., `STM32H743xx`).
5.  **Target CPU Core:** The ARM Cortex core used by this family (e.g., `cortex-m7`, `cortex-m4`).

---

## 2. Architectural Execution Plan

Once you have gathered the prerequisites, follow this exact implementation sequence. The repository scaffolding (the `.cmake` recipe and `src/` directory) already exists; your job is to populate it correctly.

### Step 1: Create the Vendor Configuration Header
ST's HAL architecture strictly requires a `stm32<fam>xx_hal_conf.h` file. This file enables the peripheral modules and defines oscillator defaults.
*   **Location:** Create this file in `src/stm32/<family>/` (e.g., `src/stm32h7/stm32h7xx_hal_conf.h`).
*   **Content Strategy:** Do NOT guess the required macros. Locate the vendor's `stm32<fam>xx_hal_conf_template.h` file inside the fetched SDK's `Inc/` directory (e.g., in `build/<preset>/_deps/vendor_stm32cubef4-src/Drivers/STM32F4xx_HAL_Driver/Inc/`). Use its content as the absolute source of truth for all required macros (e.g., `TICK_INT_PRIORITY`, `HSE_STARTUP_TIMEOUT`, `LSI_VALUE`, etc.).
*   **Module Selection:** You MUST ensure ALL peripheral modules are enabled by default (`HAL_UART_MODULE_ENABLED`, `HAL_SPI_MODULE_ENABLED`, etc.).
*   **Formatting:** Clean up the ST template code to match the project's coding style (indentation, spacing) while keeping the actual logic and macro values intact.

### Step 2: Update the Port Recipe (`cmake/ports/stm32/<family>.cmake`)
You must refactor the existing generated `.cmake` file to properly fetch and compile the Vendor SDK.

1.  **FetchContent Configuration:** Update the `FetchContent_Declare` block with the GitHub URL and Tag provided by the user.
2.  **Validation Guard:** You MUST add a `FATAL_ERROR` check ensuring `CAFFEINE_MCU_MACRO` is defined. This prevents cryptic missing file errors later.
3.  **Include Paths:** Ensure the recipe includes both the Vendor SDK's `Inc/` directories and the `src/stm32/<family>/` directory (so the SDK can find the `hal_conf.h` you just created).
4.  **Dynamic Startup Assembly:** Bare-metal STM32 requires a specific `.s` startup file (e.g., `startup_stm32h743xx.s`). You must use CMake string manipulation (`string(TOLOWER "${CAFFEINE_MCU_MACRO}" LOWERCASE_MCU_MACRO)`) to dynamically locate the correct assembly file inside the fetched SDK's `gcc/` template folder. Ensure your variable names match perfectly!
5.  **Isolate Vendor Sources:** Create a separate `add_library(vendor_sdk OBJECT ...)` target to compile the vendor SDK. Apply `target_compile_options(vendor_sdk PRIVATE -w)` to relax strict project warnings. You must include:
    *   System files: `stm32<fam>xx_hal.c`, `stm32<fam>xx_hal_cortex.c`, `stm32<fam>xx_hal_rcc.c`, `system_stm32<fam>xx.c`.
    *   **Peripheral files:** You MUST comprehensively list ALL required peripheral source files (e.g., `_hal_uart.c`, `_hal_gpio.c`, `_hal_dma.c`, `_hal_i2c.c`, `_hal_spi.c`, `_hal_tim.c`, etc.). Do not just list a few placeholders; check what the target SDK provides and include the standard peripherals.
    *   The dynamically resolved `.s` assembly file.
6.  **Main Port Target:** Link the isolated vendor target to the main project: `target_link_libraries(${PROJECT_NAME} PRIVATE $<TARGET_OBJECTS:vendor_sdk>)`.

### Step 3: Create a Build Preset
To verify your CMake logic, you must create a testable configuration in a modular preset file.
*   **Location:** Add the preset to `cmake/presets/stm32/<family>.json`. Create the file if it doesn't exist.
*   **Inheritance:** Ensure the file includes `../base.json`.
*   **Content:** Add a new object to the `configurePresets` array (e.g., `<family>-release`).
*   Inherit from `"base-arm"`.
*   Set `"CAFFEINE_VENDOR": "stm32"`.
*   Set `"CAFFEINE_PORT_FAMILY"` to the target family.
*   Set `"CAFFEINE_MCU_MACRO"` to the specific silicon variant provided by the user.
*   Set `"CAFFEINE_MCU_CORE"` to the correct ARM core.
*   *Note: FPU flags (`CAFFEINE_MCU_COMPILE_OPTIONS`) and linker scripts (`CAFFEINE_BOARD_LINKER`) can be mocked or omitted for basic build verification if not provided by the user.*

### Step 4: Implement the VMT Wrappers (C Code)
Once the build system successfully fetches and links the Vendor SDK, begin implementing the generic Caffeine APIs in `src/stm32/<family>/cfn_hal_*_port.c`.

**CRITICAL MANDATE - EXHAUSTIVE PERIPHERAL PORTING:** You MUST implement the VMT wrappers for **ALL** standard peripherals supported by the generic framework (e.g., GPIO, DMA, I2C, SPI, ADC, DAC, TIM, RTC, CAN, USB, ETH), not just UART. If the vendor CMSIS header supports the peripheral, you must create its corresponding `cfn_hal_<peripheral>_port.c` and `cfn_hal_<peripheral>_port.h` file.

**Crucial Time-Saver:** Do not write the VMT signatures from scratch! A complete set of stubs for every peripheral exists in `src/template/`.
1.  Copy the relevant template files directly (e.g., `cp src/template/cfn_hal_*_port.c src/stm32/<family>/`).
2.  Implement the logic inside the provided static functions for each peripheral.
3.  Ensure your specific vendor headers are included.

*   **Nested VMT Structure:** You MUST implement the VMT using the nested `.base` struct for generic functions.
    ```c
    static const cfn_hal_uart_api_t uart_api = {
        .base = {
            .init = port_uart_init,
            .deinit = port_uart_deinit,
            // ... other base functions
        },
        .tx_polling = stm32_uart_tx_polling,
        // ... uart-specific functions
    };
    ```

*   **Exhaustive Implementation (No MVPs):** You MUST fully implement every function in the Virtual Method Table. Do not create partial "Minimum Viable Product" stubs that return `CFN_HAL_ERROR_NOT_SUPPORTED` just to get the code to compile. Every capability defined in the generic configuration struct (e.g., data bits, parity, stop bits, flow control) must be mapped to the vendor struct. Every generic function (polling, IRQ, DMA, aborts) must be mapped to its corresponding vendor function. You may only return `CFN_HAL_ERROR_NOT_SUPPORTED` if the underlying physical silicon genuinely lacks the hardware capability.

*   **Exhaustive Hardware Superset:** When defining the `cfn_hal_<peripheral>_port_t` enum, you MUST NOT guess the available instances. You MUST search the fetched vendor CMSIS headers (e.g., `grep -rhE "^#define (USART|UART)[0-9]+ .*_BASE" _deps/.../Include/`) to discover the **absolute maximum superset** of instances available across the entire target MCU family (e.g., UART1 through UART10 for STM32F4). The enum and mapping arrays must encompass this entire superset.
*   **Hardware Instance Mapping:** Inside the `cfn_hal_<peripheral>_port.c` file, define a static mapping array that links the project's physical enum to the vendor's physical memory address. You MUST use `#ifdef` guards (e.g., `#if defined(UART4)`) around every single element in this array. This ensures the library compiles safely even if a specific user's chip variant (e.g., STM32F401) is missing a peripheral from the superset (like UART4).
*   **Global Clock Gating:** You MUST implement clock enablement via the centralized global helper `cfn_hal_port_clock_enable_gate()` defined in `cfn_hal_clock_port.c`. 
    1.  First, add the required enum definitions to `cfn_hal_port_peripheral_id_t` in `cfn_hal_clock_port.h` (mapping the exhaustive superset).
    2.  Update the `#ifdef` guarded `switch` statements inside `cfn_hal_clock_port.c` to call the appropriate `__HAL_RCC_<PERIPHERAL>_CLK_ENABLE()` macros.
*   **Low-Level Initialization (Pins & Clocks):** You MUST implement a static `low_level_init` and `low_level_deinit` function inside the port driver. This function must:
    1.  Call the global `cfn_hal_port_clock_enable_gate()` helper to turn on the peripheral's RCC clock.
    2.  Check the generic PHY struct (e.g., `driver->phy->tx`) and natively initialize any provided generic GPIO objects using the abstract `cfn_hal_gpio_init()` API.
    3.  Be called at the very beginning of the `_init` VMT wrapper before the ST HAL is invoked. Do NOT rely on ST's `HAL_<Peripheral>_MspInit` weak callbacks.
*   **Static Arrays:** Define a static array of ST handles (e.g., `static UART_HandleTypeDef port_huarts[CFN_HAL_UART_PORT_MAX];`). 
*   **O(1) Access:** Cast `driver->phy->port` back to a `uint32_t` integer to instantly index the array in your wrapper functions. Do NOT use a `switch` statement for every function call.
*   **Initialization Logic:** Inside the `_construct` function, perform a bounds check on the enum and then assign the instance directly from the mapping array: `huart->Instance = port_instances[port_id];`.
*   **Constructor Signature:** The `_construct` signature MUST match the refactored HAL and include the `phy` pointer.
    ```c
    cfn_hal_error_code_t cfn_hal_uart_construct(cfn_hal_uart_t *driver, 
                                                const cfn_hal_uart_config_t *config, 
                                                const cfn_hal_uart_phy_t *phy);
    ```
*   **Type Identity:** You MUST manually set `driver->base.type` and `driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;` in the `_construct` function.
*   **Error Translation:** Use the centralized `cfn_hal_stm32_map_error()` utility from `src/stm32/cfn_hal_stm32_error.h` to translate ST's `HAL_StatusTypeDef` to `cfn_hal_error_code_t` before returning.
*   **Bootloader Awareness:** Do not interfere with VTOR logic. The CMake recipe handles injecting `USER_VECT_TAB_ADDRESS`; your initialization code must allow the ST HAL to process it.

Example prompt
based on @caffeine-hal-ports/src/stm32/PORT_ST_SKILL.md I want to port the code to a new STM32 MCU. Create a plan for this  
port, you can prompt me to provide the information as metnioned in the skill file. You need to do a best effort to          
implement for all peripherals, do a sanity check build after each port to make sure nothing was broken. If the MCU doesn't  
support a given peripheral then you impleement it but all functions will return not supported. Make sure to adhere to the   
coding standards and philosophy of this library   