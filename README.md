<p align="center">
  <a href="https://whileone.me">
    <img src="https://raw.githubusercontent.com/while-one/caffeine-build/main/assets/logo.png" alt="Caffeine Logo" width="50%">
  </a>
<h1 align="center">The Caffeine Framework</h1>
</p>


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

# Caffeine-HAL Ports

This repository contains the hardware-specific implementations (VMT wrappers) for the **Caffeine Framework** ecosystem. It bridges the generic `caffeine-hal` APIs with vendor-specific Board Support Packages (BSPs).

## Supported Architectures & Ports

- **STM32F4 (ARM Cortex-M4):** Production-ready port supporting the series via ST's HAL (`stm32f417vg-release`). 
    - **Implemented:** GPIO, UART (w/ Idle-Line detection), SPI, I2C, CAN, ADC (DMA), DAC (DMA), Timer, ETH (MDIO/RMII), RTC (Alarms), PWM, Internal Flash (NVM), Hardware Crypto (AES/SHA/RNG), I2S Audio, SDIO.
- **GD32V (RISC-V):** Targetting GigaDevice's rv32imac architecture (`gd32vf103-release`).
- **Linux:** POSIX-compliant port for local testing and high-level applications (`linux-native`).

## Architecture & Build Infrastructure

This repository employs a clean, target-based CMake architecture driven by modular `CMakePresets.json` and a **"Hardware Contract"** model:
- **Zero Assumption:** By default, no peripherals are enabled. Every target MCU preset must explicitly declare its hardware capabilities (e.g., `CFN_HAL_UART: ON`).
- **Hierarchical Presets:** Capabilities are managed through hidden feature sets in `caffeine-build`, ensuring consistency and zero duplication across variants.
- **Dynamic Configuration:** The vendor HAL (`hal_conf.h`) and source file lists are automatically generated and synchronized with the CMake hardware contract.
- **Global Constraints:** Strict warnings (`-Werror`), `-Os` size optimization, and dead-code elimination are applied globally.
- **Linker Accuracy:** Presets use full silicon part numbers (e.g., `STM32F417VG`) to ensure accurate memory maps and linker script selection via `CAFFEINE_LINKER_SCRIPT`.

### Hardware Parameter Overrides
The framework allows applications to override default hardware parameters directly from their `CMakePresets.json` without modifying the core port files. These values are injected into the generated `stm32f4xx_hal_conf.h`.

Available override variables:
- `CFN_HAL_CLOCK_HSE_HZ`: External High Speed oscillator frequency in Hz (e.g., `8000000`).
- `CFN_HAL_CLOCK_LSE_HZ`: External Low Speed oscillator frequency in Hz (e.g., `32768`).
- `CFN_HAL_POWER_VDD_MV`: VDD voltage in millivolts (e.g., `3300`).

Example `cacheVariables` in an application's `CMakePresets.json`:
```json
"cacheVariables": {
  "CFN_HAL_CLOCK_HSE_HZ": "8000000",
  "CFN_HAL_POWER_VDD_MV": "3300"
}
```

### Direct IRQ Delegation
If your application requires custom, low-latency, or specialized interrupt handlers, you can bypass the framework's default ISRs by defining the `CFN_APP_DIRECT_IRQ` list in your CMake configuration.

Example `cacheVariables` in your `CMakePresets.json`:
```json
"cacheVariables": {
  "CFN_APP_DIRECT_IRQ": "uart;gpio;timer"
}
```
When this is set, the framework will exclude its own handlers for those specific peripherals, allowing you to safely define standard ISRs (e.g., `void USART1_IRQHandler(void)`) in your application code without linker conflicts.

### Overriding the Default Linker Script
By default, the HAL Ports library automatically propagates a standard memory map to your application via the `CAFFEINE_LINKER_SCRIPT` cache variable defined in your preset.

If your application requires a custom memory layout (e.g., for a bootloader offset), you can override this behavior by simply **omitting** `CAFFEINE_LINKER_SCRIPT` from your `CMakePresets.json` (or setting it to an empty string) and explicitly defining it in your application's `CMakeLists.txt`:

```cmake
# 1. Fetch and link the HAL (No default linker script is injected)
target_link_libraries(my_app PRIVATE caffeine::hal-ports)

# 2. Apply your custom linker script
target_link_options(my_app PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/my_custom_linker.ld)
```

---

## Development & Analysis

The project includes built-in targets for maintaining code quality:

*   **Format Code:** `cmake --build build/tests-native --target caffeine-hal-ports-format`
*   **Run Static Analysis:** `cmake --build build/tests-native --target caffeine-hal-ports-analyze`
*   **Run Unit Tests:** `cmake --build build/tests-native --target caffeine-hal-ports-test`

---

## Build Environment (Docker & Local)

To ensure consistency between local development and CI, all build environments are managed via the **`caffeine-build`** submodule. Docker images are pre-built and hosted by the central build repository.

### 1. Building with Docker (Recommended for CI Parity)

Use the `caffeine-build/scripts/build.sh` helper script to build your project inside a Docker container. This guarantees your build environment is identical to the CI.

```bash
# Build using the native Linux stage (default: builds all targets for the preset)
./caffeine-build/scripts/build.sh linux-native

# To build a specific CMake target (e.g., 'caffeine-hal-ports-format')
./caffeine-build/scripts/build.sh linux-native caffeine-hal-ports-format

# To build for a specific cross-compiler preset and a specific target
./caffeine-build/scripts/build.sh stm32f407-release caffeine-hal-ports-analyze
```

### 2. Building Natively (Without Docker)

You can still build the project directly on your host machine without Docker. You will need to install the required cross-compilers and tools manually as referenced in the `Dockerfile` located in the `caffeine-build` submodule.

```bash
# Example native build with a preset (assuming arm-none-eabi-gcc is in path)
mkdir build && cd build
cmake --preset stm32f407-release
cmake --build build/stm32f407-release
```

---

## CI Pipeline (GitHub Actions)

The CI pipeline for this repository leverages the Docker images hosted on the **`caffeine-build`** GitHub Container Registry (`ghcr.io`).

*   **Fast & Consistent Builds:** By running jobs inside pre-built Docker containers, the CI avoids redundant installations and environment drift.
*   **Submodule-Aware:** Every CI job recursively initializes submodules to fetch the required build presets and toolchain definitions.

---

## Directory Structure

- `caffeine-build/`: Submodule containing centralized build presets, toolchains, and scripts.
- `cmake/ports/<vendor>/`: Silicon-specific recipes organized by manufacturer.
- `src/`: VMT implementations mapping generic APIs to vendor code.
- `linker/`: Target for dynamically generated device-specific linker scripts.

## Coding Standards

All contributions must adhere to the ecosystem standards defined in `caffeine-build`:
- Strict C11 (No C++ or GNU extensions).
- No dynamic memory allocation (`malloc`/`free`).
- Allman-style braces and 120-column limit.
- All vendor error codes must be translated to `cfn_hal_error_code_t`.

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
