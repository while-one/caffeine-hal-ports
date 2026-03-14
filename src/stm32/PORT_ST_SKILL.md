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
ST's HAL architecture strictly requires a `stm32<fam>xx_hal_conf.h` file.
*   **Location:** Create this file in `src/stm32/<family>/` (e.g., `src/stm32h7/stm32h7xx_hal_conf.h`).
*   **Content Strategy:** Do NOT guess the required macros. Locate the vendor's `stm32<fam>xx_hal_conf_template.h` file inside the fetched SDK's `Inc/` directory. Use its content as the absolute source of truth.
*   **Module Selection:** You MUST ensure ALL peripheral modules are enabled by default (`HAL_UART_MODULE_ENABLED`, `HAL_SPI_MODULE_ENABLED`, etc.).
*   **Legacy Modules (CRITICAL):** You MUST disable/comment out all legacy modules (e.g., `HAL_CAN_LEGACY_MODULE_ENABLED`, `HAL_ETH_LEGACY_MODULE_ENABLED`) to avoid missing header errors from the SDK.
*   **Formatting:** Clean up the ST template code to match the project's coding style (indentation, spacing) while keeping the actual logic and macro values intact.
*   **Analysis Exclusion:** Any file with `stm32` in its name is automatically ignored by `cppcheck` and `clang-tidy` as configured in the root `CMakeLists.txt`. This ensures vendor headers and error mappers do not pollute analysis results.

### Step 2: Update the Port Recipe (`cmake/ports/stm32/<family>.cmake`)
You must refactor the existing generated `.cmake` file to properly fetch and compile the Vendor SDK.

1.  **FetchContent Configuration:** Update the `FetchContent_Declare` block with the GitHub URL and Tag provided by the user.
2.  **Validation Guard:** You MUST add a `FATAL_ERROR` check ensuring `CAFFEINE_MCU_MACRO` is defined.
3.  **Include Paths (CRITICAL):** Ensure the recipe includes both the Vendor SDK's `Inc/` directories and the `src/stm32/<family>/` directory. You MUST mark Vendor SDK directories as `SYSTEM` to prevent `clang-tidy` from reporting warnings in vendor code.
    ```cmake
    target_include_directories(vendor_sdk SYSTEM PRIVATE
        ${SDK_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
        # ... other vendor paths ...
    )
    ```
4.  **Dynamic Startup Assembly:** Bare-metal STM32 requires a specific `.s` startup file. Use CMake string manipulation to dynamically locate it.
5.  **Isolate Vendor Sources:** Create a separate `add_library(vendor_sdk OBJECT ...)` target to compile the vendor SDK. Apply `target_compile_options(vendor_sdk PRIVATE -w)` to relax strict project warnings.
6.  **Main Port Target:** Link the isolated vendor target to the main project: `target_link_libraries(${PROJECT_NAME} PRIVATE $<TARGET_OBJECTS:vendor_sdk>)`.

### Step 3: Create a Build Preset
To verify your CMake logic, you must create a testable configuration in a modular preset file.
*   **Location:** Add the preset to `cmake/presets/stm32/<family>.json`.
*   **Inheritance:** Ensure the file includes `../base.json`.
*   **Content:** Add a new object to the `configurePresets` array inheriting from `"base-arm"`.

### Step 4: Implement the VMT Wrappers (C Code)
Once the build system successfully fetches and links the Vendor SDK, begin implementing the generic Caffeine APIs in `src/stm32/<family>/cfn_hal_*_port.c`.

**CRITICAL MANDATE - EXHAUSTIVE PERIPHERAL PORTING:** You MUST implement the VMT wrappers for **ALL** standard peripherals supported by the generic framework.

*   **Nested VMT Structure:** You MUST implement the VMT using the nested `.base` struct for generic functions.
*   **Exhaustive Implementation:** You MUST fully implement every function in the Virtual Method Table. Do not create partial "MVP" stubs.
*   **Exhaustive Hardware Superset:** Search the fetched vendor CMSIS headers to discover the **absolute maximum superset** of instances available across the entire target MCU family.
*   **Enum Naming Rule:** You MUST name enum elements to match CMSIS definitions exactly (e.g., `CFN_HAL_UART_PORT_USART1`, `CFN_HAL_UART_PORT_UART4`, `CFN_HAL_UART_PORT_LPUART1`).
*   **Hardware Instance Mapping:** Inside the `cfn_hal_<peripheral>_port.c` file, define a static mapping array that links the project's physical enum to the vendor's physical memory address using `#ifdef` guards for each element.
*   **Global Clock Gating:** You MUST implement clock enablement via the centralized global helper `cfn_hal_port_clock_enable_gate()`.
*   **Low-Level Initialization:** Implement a static `low_level_init` function inside each port driver to gate clocks and initialize generic GPIO objects via the `cfn_hal_base_init(driver->phy->pin, CFN_HAL_PERIPHERAL_TYPE_GPIO)` call.
*   **Static Arrays:** Define a static array of ST handles (e.g., `static UART_HandleTypeDef port_huarts[CFN_HAL_UART_PORT_MAX];`). 
*   **O(1) Access:** Cast `driver->phy->instance` (the abstract enum) back to a `uint32_t` integer to index the handle array.
*   **Constructor Signature:** The `_construct` signature MUST match the refactored HAL and use the `phy->instance` member.
*   **Type Identity:** You MUST manually set `driver->base.type` and `driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;` in the `_construct` function.
*   **Exhaustive Error Translation:** Use the centralized `cfn_hal_stm32_map_error()` utility from `src/stm32/cfn_hal_stm32_error.h`.
    *   **Rule:** You MUST NOT redefine `HAL_StatusTypeDef` in the error mapper; include the ST header and use `#ifndef HAL_OK` if guarding is required for some reason.
    *   **Rule:** Use `CFN_HAL_ERROR_TIMING_TIMEOUT` for the ST `HAL_TIMEOUT` status.
*   **Bootloader Awareness:** Do not interfere with VTOR logic. The CMake recipe handles injecting `USER_VECT_TAB_ADDRESS`; your initialization code must allow the ST HAL to process it.

### Step 5: Code Quality & Analysis
All new port implementations MUST pass the project's strict quality checks before submission.
1.  **Format:** Run the format target to ensure BARR-C:2018 style compliance:
    `cmake --build build/<preset> --target caffeine-hal-ports-format`
2.  **Analyze:** Run the full static analysis suite (Cppcheck & Clang-Tidy) and resolve ALL warnings and errors:
    `cmake --build build/<preset> --target caffeine-hal-ports-analyze`
    *Note: The analyzer is configured to ignore vendor configuration headers (`_hal_conf.h`) and uses a strict header filter (`--header-filter="cfn_hal_.*"`) to ensure it only analyzes project-compliant headers.*
