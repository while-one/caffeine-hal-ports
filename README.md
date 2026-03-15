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
  <a href="https://github.com/while-one/ccaffeine-hal-ports/commits/main">
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

## Architecture & Toolchains

This repository employs a clean, target-based CMake architecture driven by modular `CMakePresets.json`:
- **Global Constraints:** Strict warnings (`-Werror` by default), `-Os` size optimization, and dead-code elimination preparation are applied globally.
- **Modular Presets (`cmake/presets/`):** MCU and board profiles are isolated by vendor and family (e.g., `cmake/presets/stm32/stm32f4.json`), allowing clean, conflict-free scalability.
- **Toolchains (`cmake/toolchains/`):** Purely configure executable paths and generic embedded linking specs (e.g., `newlib-nano` and syscall stubs for ARM).
- **Port Recipes (`cmake/ports/`):** Define the silicon-specific flags (e.g., `-mcpu`, `-march`), fetch vendor SDKs via `FetchContent` (no submodules), and compile the VMT wrappers. Vendor SDK headers are isolated as `SYSTEM` to ensure static analysis targets (Clang-Tidy/Cppcheck) focus exclusively on project code.

---

## Build Environment (Docker & Local)


To ensure consistency between local development and CI, a multi-stage `Dockerfile` is provided at the repository root. This Docker image pre-installs all necessary toolchains and dependencies, including a pre-built GoogleTest.

### 1. Building with Docker (Recommended for CI Parity)

Use the `scripts/build-local.sh` helper script to build your project inside a Docker container. This guarantees your build environment is identical to the CI.

```bash
# Build using the native Linux stage (default: builds all targets for the preset)
./scripts/build-local.sh linux-native

# To build a specific CMake target (e.g., 'caffeine-hal-ports-format')
./scripts/build-local.sh linux-native caffeine-hal-ports-format

# To build for a specific cross-compiler preset and a specific target
./scripts/build-local.sh stm32f407-release caffeine-hal-ports-analyze

# To run code coverage inside Docker
./scripts/build-local.sh tests-native caffeine-hal-ports-coverage
```

### 2. Building Natively (Without Docker)

You can still build the project directly on your host machine without Docker. The CMake configuration is designed to gracefully handle missing dependencies.

*   **GTest:** If GoogleTest is not found on your system, CMake will automatically download it via `FetchContent`.
*   **Toolchains:** You will need to install your own `cmake`, `build-essential`, `gcc-arm-none-eabi`, etc. The `Dockerfile` provides a reference for the required packages.

```bash
# Example native build with a preset
mkdir build && cd build
cmake --preset linux-native
cmake --build build/linux-native
ctest --test-dir build/linux-native --output-on-failure
```

### 3. Local Development (Cross-Repository)

For local development involving both `caffeine-hal` and `caffeine-hal-ports`, use `CMakeUserPresets.json` to link to your local `caffeine-hal` clone. This file is ignored by Git.

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "linux-native-local",
      "inherits": "linux-native",
      "displayName": "Linux Native (Local HAL)",
      "cacheVariables": {
        "FETCHCONTENT_SOURCE_DIR_CAFFEINE-HAL": "${sourceDir}/../caffeine-hal"
      }
    },
    {
      "name": "stm32f407-release-local",
      "inherits": "stm32f407-release",
      "displayName": "STM32F407 (Local HAL)",
      "cacheVariables": {
        "FETCHCONTENT_SOURCE_DIR_CAFFEINE-HAL": "${sourceDir}/../caffeine-hal"
      }
    }
  ]
}
```

---

## CI Pipeline (GitHub Actions)

The CI pipeline for this repository leverages the Docker images defined in the root `Dockerfile` and hosted on GitHub Container Registry (`ghcr.io`).

*   **Fast & Consistent Builds:** By running jobs inside pre-built Docker containers, the CI avoids redundant `apt-get install` steps and GoogleTest downloads, leading to faster and more reliable runs.
*   **Automatic Image Publishing:** A dedicated `docker-publish.yml` workflow automatically builds and pushes updated Docker images to GHCR whenever changes to the `Dockerfile` are detected. This ensures the CI always uses the latest environment.

---

## Directory Structure

- `cmake/presets/<vendor>/`: Modular `CMakePresets.json` configurations isolated by manufacturer and family.
- `cmake/toolchains/`: Compiler discovery and bare-metal linking defaults for cross-compilation.
- `cmake/ports/<vendor>/`: Silicon-specific recipes organized by manufacturer (e.g., `stm32`, `gigadevice`).
- `src/`: VMT implementations mapping generic APIs to vendor code (e.g., `src/stm32f4/`).
- `linker/`: Target for dynamically generated device-specific linker scripts.
- `vendor/`: Dynamically populated directory for vendor SDKs via CMake `FetchContent`.

## Coding Standards

All contributions must adhere to the [Caffeine-HAL SKILL.md](https://github.com/while-one/caffeine-hal/blob/main/SKILL.md) guidelines:
- Strict C11 (No C++ or GNU extensions).
- No dynamic memory allocation (`malloc`/`free`).
- Allman-style braces and 120-column limit.
- All vendor error codes must be translated to `cfn_hal_error_code_t`.

## Shared Ecosystem Standards

By instantiating a repository from this template, you inherit the framework's strict coding standards and CI infrastructure:

*   **Formatting (`config/.clang-format`):** Enforces a 120-column limit, 4-space indentation, and Allman-style braces.
*   **Static Analysis (`config/.clang-tidy`):** Enforces strict C11 compliance, memory safety rules (no dynamic allocation), and best practices for embedded systems.
*   **GitHub Infrastructure (`.github/`):** Pre-configured Pull Request templates, repository ownership rules, and baseline Continuous Integration workflows.

## The Caffeine Framework Layers

Caffeine-HAL is the foundational interface within the broader **Caffeine Framework**—a strictly decoupled, layered architecture designed for modern embedded systems. This separation of concerns ensures that business logic remains completely portable across different microcontrollers and even host operating systems.

The framework is composed of the following distinct layers:

1.  **Generic Interface (`caffeine-hal`):** This repository. Header-only definitions of the Hardware Abstraction Layer and Virtual Method Tables (VMTs).
2.  **Hardware Ports ([`caffeine-hal-ports`](https://github.com/while-one/caffeine-hal-ports)):** The concrete implementations of the HAL for specific vendors (e.g., STM32, NXP, nRF, TI) and OS environments (Linux POSIX). It encapsulates vendor SDKs and provides modern CMake cross-compilation presets.
3.  **Middleware (TBD):** Device drivers (e.g., displays, sensors) and protocols (e.g., Modbus, USB stacks) that build strictly upon the generic `caffeine-hal` interface, remaining completely agnostic to the underlying hardware.
4.  **Application (TBD):** The top-level business logic, state machines, and system orchestration that utilize the middleware and HAL interfaces.

---

## Support

They say dealing with abstraction is a form of art, so I suppose that makes me an artist? Whether this caffeine fuels an elegant HAL or a deep debugging session, I appreciate you being part of the gallery.

If my projects helped you, buy me a brew or if the opposite open a PR!

<a href="https://www.buymeacoffee.com/whileone" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-blue.png" alt="Buy Me A Coffee" style="height: 60px !important;width: 217px !important;" ></a>

---

## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.
