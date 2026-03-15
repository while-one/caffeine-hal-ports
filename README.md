<p align="center">
  <a href="https://whileone.me">
    <img src="https://whileone.me/images/caffeine-small.png" alt="Caffeine Logo" width="384" height="384">
  </a>
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

This repository employs a clean, target-based CMake architecture driven by modular `CMakePresets.json` provided via the **`caffeine-build`** submodule:
- **Global Constraints:** Strict warnings (`-Werror` by default), `-Os` size optimization, and dead-code elimination preparation are applied globally.
- **Submodule Presets:** Base profiles and toolchains are centralized in the `caffeine-build` repository. Local presets inherit from these bases to define silicon-specific macros.
- **Port Recipes:** Define silicon-specific flags, fetch vendor SDKs via `FetchContent`, and compile VMT wrappers. Linker scripts are exported as absolute paths via `INTERFACE` properties, simplifying application-level integration.

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

## Support

They say dealing with abstraction is a form of art, so I suppose that makes me an artist? Whether this caffeine fuels an elegant HAL or a deep debugging session, I appreciate you being part of the gallery.

If my projects helped you, buy me a brew or if the opposite open a PR!

<a href="https://www.buymeacoffee.com/whileone" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-blue.png" alt="Buy Me A Coffee" style="height: 60px !important;width: 217px !important;" ></a>

---

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.
