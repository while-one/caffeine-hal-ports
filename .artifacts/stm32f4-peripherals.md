# STM32F4 Peripheral "Hardware Contract" Plan

## 1. Overview
This plan implements a clean, modern, and explicit mechanism for managing peripheral availability across STM32 variants in `caffeine-hal-ports`. It adheres to the framework's philosophy of "No Magic" by requiring each MCU/Board preset to explicitly declare its supported hardware features using a hierarchical inheritance model to eliminate duplication.

## 2. Technical strategy
We use a three-tier inheritance model using **hidden presets** to manage hardware capabilities.

### Pillar 1: The Master Manifest (`caffeine-build/cmake/presets/base.json`)
A central hidden preset `base-hardware-defaults` defaults every supported HAL module to `OFF`.

### Pillar 2: Feature Sets (Hidden Presets in `stm32f4.json`)
Group peripherals common to a specific silicon line (e.g., `stm32f417-peripheral-set`).

### Pillar 3: Concrete Target Presets (`CMakePresets.json`)
Final user-facing presets using full silicon part numbers (e.g., `STM32F417VG`) inheriting from architecture and feature sets.

## 3. Phased Implementation

### Phase 1: Infrastructure Setup (caffeine-build)
1.  **Defaults:** Add `base-hardware-defaults` to `caffeine-build/cmake/presets/base.json`.
2.  **Commit:** "build: add base hardware defaults manifest"

### Phase 2: Preset Refactoring (caffeine-build & hal-ports)
1.  **Line Sets:** Create hidden feature set presets in `caffeine-build/cmake/presets/stm32/stm32f4.json`.
2.  **Targets:** Update `caffeine-hal-ports/CMakePresets.json` to use full part numbers and hierarchical inheritance.
3.  **Commit:** "build: refactor stm32f4 presets to hierarchical hardware contract"

### Phase 3: Dynamic Build Logic (hal-ports)
1.  **Template:** Convert `stm32f4xx_hal_conf.h` to `stm32f4xx_hal_conf.h.in` using `#cmakedefine01`.
2.  **CMake Recipe:** Rewrite `stm32f4.cmake` to dynamically construct `vendor_sdk` and port source lists based on `CFN_HAL_*` flags.
3.  **Commit:** "port: implement dynamic source selection and conf generation for stm32f4"

### Phase 4: Implementation Refinement (hal-ports)
1.  **Port Wrappers:** Ensure `cfn_hal_*_port.c` files use `HAL_*_MODULE_ENABLED` macros for clean fallbacks.
2.  **Commit:** "port: update wrappers to use dynamic module enable macros"

### Phase 5: Documentation & Validation
1.  **Skill:** Update `src/stm32/PORT_ST_SKILL.md` with the new workflow requirements.
2.  **Verification:**
    *   `./caffeine-build/scripts/build.sh linux-native`
    *   `./caffeine-build/scripts/build.sh stm32f417vg-release`
    *   `./caffeine-build/scripts/build.sh tests-native`
    *   Run `format` and `analyze` targets.
3.  **Commit:** "docs: update porting guidelines and finalize hardware contract"

## 4. Why This Solves the Confusions
*   **Explicit Mapping:** Files like `cfn_hal_crypto_port.c` are only added to the build if `CFN_HAL_CRYP` is `ON`.
*   **MCU Specificity:** The F407 build will no longer compile crypto-related files, preventing register mismatch errors.
*   **User Friendliness:** Presets act as a clear declaration of board capabilities.
