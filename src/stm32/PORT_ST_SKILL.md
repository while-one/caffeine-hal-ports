# STM32 Porting Guide - Agent Skill Definition

<instructions>
You are an expert embedded C developer and CMake architect. Your task is to implement a new STM32 microcontroller family port (e.g., STM32H7, STM32L4) into the `caffeine-hal-ports` repository.

Before you modify any CMake recipes or write any C code, you MUST follow this sequence:
1. **Identify Target:** Ensure you know the target MCU family and specific MCU.
2. **Pre-flight Check:** Inspect the repository to determine the current state of requested port.
3. **Hardware Contract Awareness:** This framework uses a "zero-assumption" hardware contract. No peripherals are enabled by default.
4. **Execute:** Adapt the Architectural Execution Plan (Section 2).
</instructions>

## 1. Prerequisite Information Gathering
1.  **Target Family Name:** (e.g., `stm32h7`).
2.  **Vendor SDK Info:** GitHub URL and Git Tag.
3.  **Target MCU Macro:** (e.g., `STM32H743xx`).
4.  **Full Part Number:** (e.g., `STM32H743VIT6`).
5.  **Target CPU Core:** (e.g., `cortex-m7`).

---

## 2. Architectural Execution Plan

### Step 1: Create the Dynamic Vendor Configuration Header
*   **Location:** Create `src/stm32/<family>/stm32<fam>xx_hal_conf.h.in`.
*   **Module Selection:** Use `#cmakedefine HAL_<PERIPH>_MODULE_ENABLED` for every module.
*   **Zero Assumption:** Do NOT enable any modules by default in the template.

### Step 2: Update the Port Recipe (`cmake/ports/stm32/<family>.cmake`)
*   **Dynamic Source Selection:** Implement a mapping logic that adds vendor `.c` files and port `.c` files only when the corresponding `CFN_HAL_<PERIPH>` flag is `ON`.
*   **Header Generation:** Use `configure_file()` to generate the final `.h` from the `.in` template.
*   **Port Files:** ALWAYS include port wrapper files in the `PORT_SOURCES_LIST` to ensure constructor symbols exist, but wrap their implementation in guards.

### Step 3: Create Build Presets (Hierarchical)
*   **Master Defaults:** Ensure the target inherits from `base-hardware-defaults` (defined in `base.json`).
*   **Feature Sets:** Create hidden presets (e.g., `<family>-base-peripheral-set`) to group common peripherals.
*   **Concrete Targets:** Final presets MUST use the full silicon part number and inherit from the feature set and architecture base.

### Step 4: Implement the VMT Wrappers (C Code)
*   **Guard Implementation:** Wrap the implementation logic in `#if defined(HAL_<PERIPH>_MODULE_ENABLED)`.
*   **Clean Fallback:** Ensure the `_construct` function returns `CFN_HAL_ERROR_NOT_SUPPORTED` if the module is disabled.
*   **Exhaustive Porting:** Implement the full VMT for the peripheral.

### Step 5: Code Quality & Validation
1.  **Format:** `cmake --build build/<preset> --target caffeine-hal-ports-format`
2.  **Analyze:** `cmake --build build/<preset> --target caffeine-hal-ports-analyze`
3.  **Build:** Ensure the actual firmware build succeeds for the target.
