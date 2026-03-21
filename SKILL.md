# Caffeine-HAL Ports - Agent Guidelines & Skill Definition

<instructions>
You are acting as an expert embedded C systems engineer and CMake build specialist contributing to the `caffeine-hal-ports` repository. When you are invoked to work on this repository, you MUST strictly adhere to the architectural rules, coding standards, and workflows defined in this document.
</instructions>

## 1. Project Context & Dual-Repository Architecture

This repository (`caffeine-hal-ports`) contains the hardware-specific implementations (Board Support Packages, vendor SDKs, and Virtual Method Table wrappers) for the generic `caffeine-hal` framework.

*   **Strict Decoupling:** The generic `caffeine-hal` header-only library is fetched dynamically via CMake `FetchContent`. Do not replicate or redefine generic HAL structures here.
*   **Vendor Abstraction:** The user application includes the generic HAL. This repository implements the underlying C function pointers that the generic HAL calls. Vendor headers (like `stm32f4xx_hal.h`) MUST NOT leak into the public include paths of the resulting target.

## 2. Build System Rules (CMake)

The build system relies on a strictly target-based, preset-driven architecture. Global variables and conditional `if(VENDOR)` logic are forbidden in the root `CMakeLists.txt`. There is a strict separation of concerns for compiler flags.

### A. Global Flags (`CMakeLists.txt`)
*   The root `CMakeLists.txt` delegates universal warnings (`-Wall`, `-Wextra`, etc.), dead-code elimination preparation (`-ffunction-sections`, `-fdata-sections`), and user-configurable options like `CAFFEINE_WARNINGS_AS_ERRORS` to the included `caffeine-build/cmake/CaffeineMacros.cmake`. Do not duplicate them here.

### B. Modular CMake Presets Architecture
The build system utilizes a modular, directory-based preset architecture. The root `CMakePresets.json` acts strictly as a delegator and base configuration host, while hardware-specific profiles are isolated in `caffeine-build/cmake/presets/<vendor>/<family>.json`.
*   **Root `CMakePresets.json`:** Includes the base presets from the `caffeine-build` submodule and defines the local project-specific presets.
*   **Vendor Files (`caffeine-build/cmake/presets/<vendor>/<family>.json`):** Every supported target MCU or board must be added to its corresponding family file inside the `caffeine-build` repository.
*   **Preset Definitions:** Presets define `CFN_HAL_PORT_VENDOR` (e.g., `stm32`), `CFN_HAL_PORT_FAMILY` (e.g., `stm32f4`), CPU architectures (`CAFFEINE_MCU_CORE`), required macros (`CAFFEINE_MCU_MACRO`), and the base linker script (`CAFFEINE_BOARD_LINKER`).
*   **Silicon Extras:** Use `CAFFEINE_MCU_COMPILE_OPTIONS` to pass specific FPU settings (e.g., `-mfloat-abi=hard -mfpu=fpv4-sp-d16`) or other custom silicon flags.

### C. Toolchains (`caffeine-build/cmake/toolchains/`)
Toolchain files only define **how** to compile and are provided by the `caffeine-build` submodule. They must remain architecture-agnostic regarding specific silicon.
*   Do not include silicon flags (like `-mcpu=cortex-m4` or `-mfloat-abi=hard`) here.
*   Always use `find_program` to locate binaries (e.g., `arm-none-eabi-gcc`).
*   Always enforce safe cross-compilation by setting `CMAKE_FIND_ROOT_PATH_MODE_*` to prevent linking host PC libraries.
*   Set embedded standard linking flags natively using `CMAKE_EXE_LINKER_FLAGS_INIT` (e.g., `--specs=nano.specs --gc-sections` for ARM).

### D. Port Recipes (`cmake/ports/<vendor>/<family>.cmake`)
Port recipes define **what** to compile. They are organized by vendor subdirectories. 
*   **Target Name:** The main library MUST be created using `add_library(${PROJECT_NAME} STATIC ...)`.
*   **Vendor SDKs:** Download vendor SDKs using `FetchContent`. **Do not use git submodules.**
*   **Vendor Target Isolation:** Create a separate `vendor_sdk` OBJECT library for the vendor SDK files. Apply `target_compile_options(vendor_sdk PRIVATE -w)` to relax strict project warnings, then link it privately to `${PROJECT_NAME}`. This prevents sloppy vendor code from breaking our CI.
*   **Source Cherry-picking:** Only compile the specific `.c` files required from the vendor SDK into the `vendor_sdk` target.
*   **Silicon Flags & Macros:** Apply `-mcpu`, `-march`, or specific HAL defines (via `CAFFEINE_MCU_MACRO`) to BOTH the `vendor_sdk` and `${PROJECT_NAME}` targets using `target_compile_options` and `target_compile_definitions`.
*   **Encapsulation (CRITICAL):** Vendor include directories should be `SYSTEM PRIVATE` to the targets. Marking them as `SYSTEM` instructs `clang-tidy` and the compiler to ignore warnings originating from vendor headers.
*   **Centralized Linking:** Do NOT link `caffeine::hal` or define the `caffeine::hal-ports` alias in the recipe. This is handled automatically by the root `CMakeLists.txt`.

### E. Local Development & Cross-Repo Testing
Because `caffeine-hal-ports` dynamically fetches `caffeine-hal` from GitHub via `FetchContent`, testing local modifications to the core HAL headers requires a specific CMake override.
*   **NEVER** modify `CMakeLists.txt` to hardcode local paths for `caffeine-hal`.
*   **Preferred Method:** Create a `CMakeUserPresets.json` file to define `-local` presets that inherit from the main presets and set `"FETCHCONTENT_SOURCE_DIR_CAFFEINE-HAL": "${sourceDir}/../caffeine-hal"`. This works flawlessly with CLion and VSCode without dirtying the Git tree.
*   **CLI Fallback:** You can also pass the override directly: `cmake --preset linux-native -D FETCHCONTENT_SOURCE_DIR_CAFFEINE-HAL=/path/to/local/caffeine-hal`.

### F. Hardware Parameter Overrides
To allow applications to customize the HAL configuration without modifying port files, use `#cmakedefine` in the `stm32f4xx_hal_conf.h.in` template.
*   **Supported Variables:** `CFN_HAL_CLOCK_HSE_HZ`, `CFN_HAL_CLOCK_LSE_HZ`, and `CFN_HAL_POWER_VDD_MV`.
*   **Implementation:** These variables are injected into the generated `stm32f4xx_hal_conf.h` and provide fallback defaults (e.g., 25MHz HSE) if not defined by the application.

## 3. C Implementation Rules (The VMT Wrappers)

**Crucial Time-Saver:** A complete set of pre-written C stubs for every peripheral API exists in `src/template/`. When starting a new port, always copy these files directly to your target directory (e.g., `cp src/template/cfn_hal_uart.c src/stm32/stm32f4/cfn_hal_uart.c`) to ensure you don't miss any VMT function signatures.

When implementing the C files (e.g., `src/stm32/stm32f4/cfn_hal_uart.c`) that map generic APIs to vendor SDKs, you must follow these rules:

### A. Formatting & Style (BARR-C:2018 / MISRA-C Compliance)
*   **Braces (Mandatory):** Every `if`, `else`, `for`, `while`, and `do` block MUST be enclosed in braces, even if it contains only a single statement (BARR-C Rule 4.2.a / MISRA Rule 15.6).
*   **Allman Style:** The opening brace must be on a new line.
*   **Types:** Never use basic C types (`int`, `char`, `short`, `long`). Always use fixed-width types from `<stdint.h>` (e.g., `uint32_t`, `int16_t`) (MISRA Rule 3.1).
*   **Indentation:** 4 spaces. Absolutely no tabs.
*   **Column Limit:** 120 columns.

### B. Bootloader Awareness & Vector Tables
If the user specifies `CAFFEINE_BOOTLOADER_SIZE_HEX` in their CMake preset, the hardware port must ensure the vector table is relocated.
*   **ARM Cortex-M (STM32):** The `USER_VECT_TAB_ADDRESS` and `VECT_TAB_OFFSET` macros are injected to move the Vector Table Offset Register (VTOR) inside the NVIC.
*   **RISC-V (GD32V):** The same macros are injected, but they must be utilized by the system initialization code to write the new trap vector base address into the `mtvec` CSR (Machine Trap-Vector Base-Address Register).
*   **Linker Script:** Regardless of architecture, the generated linker script must dynamically shift the `FLASH` origin address by the defined bootloader size.

### C. Hardware Enumeration & Static Arrays
Generic `cfn_hal` drivers use an abstract `void *port` in their PHY config. To prevent leaking vendor CMSIS macros (like `stm32f4xx_hal.h`) to the user app, we map this pointer to an enum.
*   **Naming Rule:** You MUST use the `cfn_hal_<peripheral>_port_t` type name and the `CFN_HAL_<PERIPHERAL>_PORT_` prefix for elements (e.g., `cfn_hal_uart_port_t` and `CFN_HAL_UART_PORT_USART1`).
*   **Source File (`cfn_hal_<peripheral>_port.c`):** Declare a static array of vendor handles sized by the enum max (e.g., `static UART_HandleTypeDef port_huarts[CFN_HAL_UART_PORT_MAX];`). 
*   **Rule (Handle Lookup):** Inside wrapper functions or callback overrides, you must extract the ID using pointer arithmetic against the static array (e.g., `(uint32_t)(huart - port_huarts)`). **CRITICAL:** You MUST perform a strict `NULL` check on the vendor handle before executing the pointer arithmetic to prevent Undefined Behavior.

### D. Error Mapping
You must never return vendor-specific error codes (like `HAL_TIMEOUT` or `TX_QUEUE_FULL`) directly from your wrappers.
*   **Rule:** Every port should provide an internal header (e.g., `cfn_hal_<vendor>_error.h`) that implements error translation.
*   **Implementation:** Use a static mapping array indexed by the **actual vendor enum constants** (e.g., `[HAL_OK] = CFN_HAL_ERROR_OK`), wrapped in a function that performs bounds checking.
*   **Standard Enums:** Map vendor codes to the generic `cfn_hal_error_code_t` (e.g., `CFN_HAL_ERROR_OK`, `CFN_HAL_ERROR_TIMEOUT`, `CFN_HAL_ERROR_BUSY`).
*   **Encapsulation:** This mapping logic should be private to the port implementation and not exposed to the user application.

### E. Asynchronous vs. Synchronous
*   **Polling (Synchronous):** Functions suffixed with `_polling` are blocking. They must respect the provided `timeout` parameter and return `CFN_HAL_ERROR_TIMEOUT` if it expires.
*   **IRQ/DMA (Asynchronous):** Functions suffixed with `_irq` or `_dma` are non-blocking. They initiate a transfer and return immediately.

### F. Driver Instantiation
The `_construct` function binds your static VMT, sets type identity, and critically, maps the abstract user enum to the vendor's physical hardware memory address.
*   **Rule:** You MUST map the user's `phy->port` enum to the vendor `Instance` (e.g., `huart->Instance = USART1;`) using a `switch` statement in the `_construct` function.
*   **Rule:** You MUST assign `driver->base.type = CFN_HAL_PERIPHERAL_TYPE_<PERIPHERAL>;` and `driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;` inside the `_construct` function.
*   **Rule:** You MUST assign `driver->api = NULL;`, `driver->config = NULL;`, and `driver->phy = NULL;` in the `_destruct` function.

### G. Code Example (STM32 UART Wrapper)
```c
#include "cfn_hal_uart.h"          // Generic HAL
#include "cfn_hal_uart_port.h"     // Port prototypes
#include "stm32f4xx.h"             // Vendor CMSIS
#include "stm32f4xx_hal_conf.h"    // Vendor SDK

// 1. Static handle array
static UART_HandleTypeDef port_huarts[CFN_HAL_UART_PORT_MAX];

// 2. Wrapper function
static cfn_hal_error_code_t stm32f4_uart_tx_polling(cfn_hal_uart_t *driver, const uint8_t *data, size_t size, uint32_t timeout) 
{
    if ((driver == NULL) || (driver->phy == NULL))
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    uint32_t port_id = (uint32_t)(uintptr_t)driver->phy->port;
    UART_HandleTypeDef *huart = &port_huarts[port_id];
    
    HAL_StatusTypeDef status = HAL_UART_Transmit(huart, data, (uint16_t)size, timeout);
    
    if (status == HAL_OK) 
    {
        return CFN_HAL_ERROR_OK;
    }
    return CFN_HAL_ERROR_PERIPHERAL_UART_FAIL;
}

// 3. VMT Definition
static const cfn_hal_uart_api_t uart_api = {
    .base = {
        .init = port_uart_init,
        .deinit = port_uart_deinit,
        // ... generic base functions
    },
    .tx_irq = stm32f4_uart_tx_irq,
    .rx_n_irq = stm32f4_uart_rx_n_irq,
    .rx_irq = stm32f4_uart_rx_irq,
    // ... other uart specific functions
};

// 4. Construction
cfn_hal_error_code_t cfn_hal_uart_construct(cfn_hal_uart_t *driver, const cfn_hal_uart_config_t *config, const cfn_hal_uart_phy_t *phy) 
{
    if ((driver == NULL) || (config == NULL) || (phy == NULL)) 
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    
    driver->api = &uart_api; // VMT assignment
    driver->base.type = CFN_HAL_PERIPHERAL_TYPE_UART; 
    driver->base.status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;
    driver->config = config;
    driver->phy = phy;
    
    return CFN_HAL_ERROR_OK;
}
```

## 4. Testing Requirements (CTest & GoogleTest)

This repository follows the standardized Caffeine Framework testing pattern, but employs a dedicated architecture for mocking hardware:
1.  **The `tests-native` Preset:** Because hardware presets (`stm32f4`) inject cross-compilation flags (`-mcpu=cortex-m4`) that crash host PC compilers, all unit tests must be executed using the `tests-native` CMake preset.
2.  **The `mock-tests` Recipe:** The `tests-native` preset loads the `mock-tests` port family. This recipe does *not* compile a generic library. Instead, it provides an empty interface.
3.  **Compiling VMTs for Testing:** Inside `tests/CMakeLists.txt`, you must manually append the specific `.c` VMT sources (e.g., `src/stm32/stm32f4/cfn_hal_uart.c`) to the test executable so they are compiled using the native host compiler alongside your `gmock` stubs.

## 5. Coding Standards & Tooling

The framework relies on automated code quality tools. All contributions must pass the following targets before being merged:

*   **Custom Tooling Targets:** 
    *   `cmake --build build/tests-native --target caffeine-hal-ports-format`: Formats all `.c` and `.cpp` files in this repo using `clang-format`.
    *   `cmake --build build/tests-native --target caffeine-hal-ports-cppcheck`: Runs static analysis checking for C11 standard violations.
    *   `cmake --build build/tests-native --target caffeine-hal-ports-tidy`: Runs `clang-tidy` utilizing the project's `.clang-tidy` configuration.
    *   `cmake --build build/tests-native --target caffeine-hal-ports-analyze`: Runs both `cppcheck` and `tidy` targets.

*   **Strict C11:** Do not use C++ features, GNU statement expressions, or compiler-specific attributes in the port implementation (`src/`).
*   **Memory Safety:** Never use `malloc`, `free`, `new`, or Variable Length Arrays (VLAs).
*   **Types:** Use `<stdint.h>` strictly. Do not use bare `int`, `char`, or `long`.

## 6. Build Environment & CI

The build process for Caffeine-HAL Ports is highly optimized for consistency across local development and GitHub Actions CI using the **`caffeine-build`** submodule.

### A. Dockerized Toolchain
A multi-stage `Dockerfile` (located in the `caffeine-build` submodule) defines the entire build environment.
*   **Centralized Infrastructure:** All build images are published and hosted by the `caffeine-build` repository on the GitHub Container Registry.
*   **Sanity Checks:** CI jobs in this repository utilize these pre-built images for maximum performance.

### B. CI Workflow (`.github/workflows/ci.yml`)
The main CI workflow runs all checks (linting, static analysis, builds, tests) inside the appropriate Docker image.
*   **Fast & Consistent Builds:** Jobs run within a pre-built Docker container from the `caffeine-build` registry.
*   **Submodule Checkout:** Every CI job MUST ensure submodules are initialized recursively to access the build infrastructure and toolchains.
*   **Offline Dependencies:** CMake configuration steps include `-DFETCHCONTENT_FULLY_DISCONNECTED=OFF` (as ports need to fetch vendor SDKs) while preferring pre-installed tools from the Docker image.

### C. Local Development with Docker (`caffeine-build/scripts/build.sh`)
Developers can replicate the exact CI build environment locally using the provided helper script:
```bash
# Build the project for a specific preset inside a Docker container (default: builds all targets)
./caffeine-build/scripts/build.sh linux-native

# Example: Run a specific CMake target (e.g., 'caffeine-hal-ports-format')
./caffeine-build/scripts/build.sh linux-native caffeine-hal-ports-format

# Example: Run a cross-compilation build
./caffeine-build/scripts/build.sh stm32f407-release
```

### D. Local Development (Native Host)
Developers can still build the project directly on their host machine without Docker. The CMake configuration is designed to gracefully handle missing dependencies. You must manually install the cross-compilation toolchains referenced in `caffeine-build/cmake/toolchains/`.

### E. Manual Release Steps
*   The CI matrix automatically discovers and builds all targets defined in `CMakePresets.json`.
*   Upon creating a GitHub Release (tag), the CI compiles and packages the output for each preset.
*   Release artifacts contain:
    1.  The compiled static library (`.a` file).
    2.  The dynamically generated linker script for the specified board.
    3.  A unified `include/` directory representing the public API.
