# Caffeine-HAL Ports

This repository contains the hardware-specific implementations (VMT wrappers) for the **Caffeine Framework** ecosystem. It bridges the generic `caffeine-hal` APIs with vendor-specific Board Support Packages (BSPs).

## Supported Architectures & Ports

- **STM32F4 (ARM Cortex-M4):** Targetting STM32F4 series via ST's HAL (`stm32f407-release`).
- **GD32V (RISC-V):** Targetting GigaDevice's rv32imac architecture (`gd32vf103-release`).
- **Linux:** POSIX-compliant port for local testing and high-level applications (`linux-native`).

## Architecture & Toolchains

This repository employs a clean, target-based CMake architecture driven by modular `CMakePresets.json`:
- **Global Constraints:** Strict warnings (`-Werror` by default), `-Os` size optimization, and dead-code elimination preparation are applied globally.
- **Modular Presets (`cmake/presets/`):** MCU and board profiles are isolated by vendor and family (e.g., `cmake/presets/stm32/stm32f4.json`), allowing clean, conflict-free scalability.
- **Toolchains (`cmake/toolchains/`):** Purely configure executable paths and generic embedded linking specs (e.g., `newlib-nano` and syscall stubs for ARM).
- **Port Recipes (`cmake/ports/`):** Define the silicon-specific flags (e.g., `-mcpu`, `-march`), fetch vendor SDKs via `FetchContent` (no submodules), and compile the VMT wrappers.

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
