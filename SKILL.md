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
- **Mock Interface:** Automatically links against `caffeine::hal-mock` when `CFN_BUILD_TESTS=ON`.
- **VMT Logic Verification (Native):** To test hardware-specific logic natively, use the **Mock SDK** pattern (refer to `.artifacts/native_port_testing_plan.md`). This allows compiling port source files using the host compiler by providing mocked vendor headers and function stubs.
- **Test Execution:** Tests are executed using the `unit-tests-gtest` test preset.
