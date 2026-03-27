<p align="center">
  <a href="https://whileone.me">
    <img src="https://raw.githubusercontent.com/while-one/caffeine-build/main/assets/logo.png" alt="Caffeine Logo" width="50%">
  </a>
<h1 align="center">The Caffeine Framework</h1>
</p>

# Caffeine-HAL Ports

<p align="center">
  <img src="https://img.shields.io/badge/C-11-blue.svg?style=flat-square&logo=c" alt="C11">
  <img src="https://img.shields.io/badge/CMake-%23008FBA.svg?style=flat-square&logo=cmake&logoColor=white" alt="CMake">
  <a href="https://github.com/while-one/caffeine-hal-ports/tags">
    <img src="https://img.shields.io/github/v/tag/while-one/caffeine-hal-ports?style=flat-square&label=Release" alt="Latest Release">
  </a>
  <a href="https://github.com/while-one/caffeine-hal-ports/actions/workflows/ci.yml">
    <img src="https://img.shields.io/github/actions/workflow/status/while-one/caffeine-hal-ports/ci.yml?style=flat-square&branch=main" alt="CI Status">
  </a>
  <a href="https://github.com/while-one/caffeine-hal-ports/commits/main">
    <img src="https://img.shields.io/github/last-commit/while-one/caffeine-hal-ports.svg?style=flat-square" alt="Last Commit">
  </a>
  <a href="https://github.com/while-one/caffeine-hal-ports/blob/main/LICENSE">
    <img src="https://img.shields.io/github/license/while-one/caffeine-hal-ports?style=flat-square&color=blue" alt="License: MIT">
  </a>
</p>

This repository contains the hardware-specific implementations (VMT wrappers) for the **Caffeine Framework** ecosystem. It bridges the generic `caffeine-hal` APIs with vendor-specific Board Support Packages (BSPs).

## Supported Architectures & Ports

- **STM32F4 (ARM Cortex-M4):** Production-ready port supporting the series via ST's HAL.
- **GD32V (RISC-V):** Targeting GigaDevice's rv32imac architecture.
- **Linux:** POSIX-compliant port for local testing and high-level applications (`linux-native`).

## Architecture & Hardware Contract

This repository employs an encapsulated, target-based CMake architecture driven by a **"Hardware Contract"** model:
- **Encapsulated Targets:** All low-level MCU definitions (core, FPU, compiler flags, macros, linker scripts) are contained within target-specific `.cmake` scripts (e.g., `cmake/ports/stm32/stm32f4/stm32f417vgtx.cmake`).
- **Zero Boilerplate:** Applications only need to specify their vendor, family, and target MCU to receive full hardware support.
- **Dynamic Configuration:** The vendor HAL (`hal_conf.h`) and source file lists are automatically generated based on the selected target.

### 1. Configure Hardware (Application Level)
Applications specify their hardware requirements in their `CMakePresets.json`:

```json
"cacheVariables": {
  "CFN_HAL_PORT_VENDOR": "stm32",
  "CFN_HAL_PORT_FAMILY": "stm32f4",
  "CFN_HAL_PORT_TARGET": "stm32f417vgtx"
}
```

### 2. Hardware Parameter Overrides
Applications can override default hardware parameters directly from their `CMakePresets.json`:
- `CFN_HAL_CLOCK_HSE_HZ`: External High Speed oscillator frequency (e.g., `8000000`).
- `CFN_HAL_POWER_VDD_MV`: VDD voltage in millivolts (e.g., `3300`).

---

## Development & Analysis

The project includes built-in targets for maintaining code quality:

*   **Format Code:** `cmake --build build/unit-tests-gtest --target caffeine-hal-ports-format`
*   **Run Static Analysis:** `cmake --build build/unit-tests-gtest --target caffeine-hal-ports-analyze`
*   **Run Unit Tests:** `cmake --build build/unit-tests-gtest --target caffeine-hal-ports-test` (Requires `-D CFN_BUILD_TESTS=ON`)

---

## Build Environment (Docker & Local)

Use the framework's centralized scripts for orchestration:

```bash
# Full Quality Gate (Format -> Analyze -> Build -> Test)
./caffeine-build/scripts/ci.sh all

# Build using the native Linux stage (Template Compliance mode)
./caffeine-build/scripts/build.sh unit-tests-gtest

# Perform a clean build for a specific cross-compiler preset
./caffeine-build/scripts/build.sh --clean stm32f407-release
```

### Native Logic Testing (Experimental)
To test hardware-specific VMT logic on your host without an ARM toolchain, use the **Mock SDK** pattern. This allows compiling the exact `.c` port files by providing a "fake" vendor environment. See `.artifacts/native_port_testing_plan.md` for details.

---

## Directory Structure

- `caffeine-build/`: Submodule containing centralized build presets, toolchains, and scripts.
- `cmake/ports/<vendor>/<family>/`: Hardware target definitions.
- `src/<vendor>/<family>/`: VMT implementations mapping generic APIs to vendor code.
- `linker/`: device-specific linker scripts.

---

## Support the Gallery

While this library is no Mondrian, it deals with a different form of **abstraction art**. Hardware abstraction is a craft of its own—one that keeps your application code portable and your debugging sessions short.

Whether **Caffeine** is fueling an elegant embedded project or just helping you wake up your hardware, you can contribute in the following ways:

* **Star & Share:** If you find this project useful, give it a ⭐ on GitHub and share it with your fellow firmware engineers. It helps others find the library and grows the Caffeine community.
* **Show & Tell:** If you are using Caffeine in a project (personal or professional), **let me know!** Hearing how it's being used is a huge motivator.
* **Propose Features:** If the library is missing a specific "brushstroke," let's design the interface together.
* **Port New Targets:** Help us expand the collection by porting the HAL to new silicon or peripheral sets.
* **Expand the HIL Lab:** Contributions go primarily toward acquiring new development boards. These serve as dedicated **Hardware-in-the-Loop** test targets, ensuring every commit remains rock-solid across our entire fleet of supported hardware.

**If my projects helped you, feel free to buy me a brew. Or if it caused you an extra debugging session, open a PR!**

<a href="https://www.buymeacoffee.com/whileone" target="_blank">
  <img src="https://img.shields.io/badge/Caffeine%20me--0077ff?style=for-the-badge&logo=buy-me-a-coffee&logoColor=white" 
       height="40" 
       style="border-radius: 5px;">
</a>&nbsp;&nbsp;&nbsp;&nbsp;
<a href="https://github.com/sponsors/while-one" target="_blank">
<img src="https://img.shields.io/badge/Sponsor--ea4aaa?style=for-the-badge&logo=github-sponsors" height="40" style="border-radius: 5px;"> </a>&nbsp;&nbsp;&nbsp;
<a href="https://github.com/while-one/caffeine-hal-ports/compare" target="_blank">
<img src="https://img.shields.io/badge/Open%20a%20PR--orange?style=for-the-badge&logo=github&logoColor=white" height="40" style="border-radius: 5px;">
</a>

---

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.
