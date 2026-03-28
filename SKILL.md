# Caffeine-HAL Ports - Agent Guidelines & Skill Definition

## 1. Project Context
This repository contains the hardware-specific implementations (VMT wrappers) for the Caffeine Framework. It bridges generic HAL APIs with vendor-specific SDKs.

### Core Architecture
- **Encapsulated Targets:** All low-level hardware definitions (core, flags, linker scripts) are encapsulated in `.cmake` target scripts.
- **Hardware Contract:** Applications only specify their `CFN_HAL_PORT_TARGET`.
- **Target Selection:** Handled via the `CFN_HAL_PORT_VENDOR`, `CFN_HAL_PORT_FAMILY`, and `CFN_HAL_PORT_TARGET` variables.

---

## 2. Porting Standards

### A. Directory Structure
- `cmake/ports/<vendor>/<family>/<family>.cmake`: Base family logic (SDK fetching).
- `cmake/ports/<vendor>/<family>/<target>.cmake`: Specific MCU definitions.
- `src/<vendor>/<family>/`: VMT implementation files.

### B. Implementation Rules
- **No Globbing:** Port implementations should manually list their source files in the base family script.
- **O(1) Arithmetic:** Internal helpers like `get_port_id` must use pointer arithmetic, not loops.
- **Error Mapping:** Map vendor-specific errors to `cfn_hal_error_code_t`.

---

## 3. Build & CI

### A. Unified CI
Every repository utilizes the framework's quality gate script:
```bash
./caffeine-build/scripts/ci.sh all
```

### B. Strict Analysis
A **zero-warning policy** is enforced. `clang-tidy` and `cppcheck` violations will fail the build. Static analysis targets are managed via the centralized `cfn_add_code_quality_targets()` macro. Cross-compilation headers are automatically resolved via the `cfn_get_clang_tidy_extra_args` macro.

### C. Template Compliance
The default "no-hardware" mode (`unit-tests-gtest`) proactively analyzes the `template/` directory to prevent architectural rot in the boilerplate. This preset must be defined in `CMakePresets.json` inheriting from `base-unit-tests-gtest`.

---

## 4. Testing Requirements

### A. Mock Interface
Automatically links against `caffeine::hal-mock` when `CFN_BUILD_TESTS=ON`. This is used for testing hardware-agnostic logic that consumes the HAL.

### B. VMT Logic Verification (Native Mock SDK)
To verify that the hardware-specific translation logic (e.g., mapping `cfn_hal_uart_config_t` to `UART_InitTypeDef`) is correct *before* hitting silicon, use the **Native Mock SDK** pattern.

#### Architecture:
1.  **Mock Vendor SDK:** Instead of the real vendor SDK (which contains ARM assembly), we use a set of C++ Google Mock headers and sources located in `tests/<vendor>/<family>/mocks/`.
2.  **`vendor_sdk` Target:** In native mode, the `vendor_sdk` library target is populated with these C++ mocks instead of vendor C files. This maintains a clean linker graph.
3.  **Core Bypassing:** We provide a mocked `core_cm4.h` (or similar) to override ARM-specific macros (like `__ASM`) with native-compatible equivalents.

#### Critical Implementation Rules:
-   **Full Macro Coverage:** In the mock configuration (e.g., `stm32f4xx_hal_conf.h`), you MUST inject `#define` statements for **every possible peripheral instance** (e.g., `USART1` through `UART10`). This ensures the native compiler evaluates all conditionally-wrapped code blocks (`#if defined(USART1)`), even if they aren't on the base MCU.
-   **Unique RCC Gates:** Mock RCC enable macros (e.g., `__HAL_RCC_UART1_CLK_ENABLE()`) must perform a unique side-effect (e.g., assigning a unique value to a static variable). This prevents `clang-tidy` from flagging "identical branch" errors in large clock-gating switch statements.
-   **GMock Forwarding:** Mocked C-functions (e.g., `HAL_UART_Init`) should forward to a singleton GMock object. This allows test fixtures to use `EXPECT_CALL` and `SaveArg` for high-fidelity inspection.
-   **Compilation Flags:** Host-side C++ compilation of vendor headers often requires `-fpermissive` to handle legacy C-style pointer casts safely.

### C. Lessons Learned & Best Practices
-   **Linker Isolation:** Always keep the `vendor_sdk` target as the bridge. Never compile mock files directly into the test executable if they are intended to satisfy port-layer dependencies.
-   **Header Parity:** Use the *actual* vendor headers as much as possible, only mocking the "un-compilable" core headers (like CMSIS). This ensures your mocks don't drift from the real data structures.
-   **Zero-Warning Native Stage:** The native test stage MUST pass `clang-tidy` and `cppcheck`. If mock macros cause "branch-clone" warnings, make the macros unique rather than suppressing the warning.
-   **CI Isolation:** Each mock-test preset MUST define a unique `binaryDir` in `CMakePresets.json` (e.g., `${sourceDir}/build/stm32f4-mock-tests`). This prevents configuration drift and directory collisions when running multiple CI stages sequentially.

---

## 5. Test Execution
- **Generic Tests:** Run via the `unit-tests-gtest` preset.
- **Family-Specific Mock Tests:** Run via dedicated presets (e.g., `stm32f4-mock-tests`).
```bash
./caffeine-build/scripts/build.sh stm32f4-mock-tests all
ctest --preset stm32f4-mock-tests
```
