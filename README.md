<p align="center">
  <a href="https://whileone.me">
    <img src="https://whileone.me/images/caffeine-small.png" alt="Caffeine Logo" width="384" height="384">
  </a>
</p>



<p align="center">
  <img src="https://img.shields.io/badge/C-11-blue.svg?style=flat-square&logo=c" alt="C11">
  <img src="https://img.shields.io/badge/CMake-%23008FBA.svg?style=flat-square&logo=cmake&logoColor=white" alt="CMake">
  <a href="https://github.com/while-one/caffeine-hal/tags">
    <img src="https://img.shields.io/github/v/tag/while-one/caffeine-hal?style=flat-square&label=Release" alt="Latest Release">
  </a>
  <a href="https://github.com/while-one/caffeine-hal/actions/workflows/ci.yml">
    <img src="https://img.shields.io/github/actions/workflow/status/while-one/caffeine-hal/ci.yml?style=flat-square&branch=main" alt="CI Status">
  </a>
  <a href="https://github.com/while-one/caffeine-hal/commits/main">
    <img src="https://img.shields.io/github/last-commit/while-one/caffeine-hal.svg?style=flat-square" alt="Last Commit">
  </a>
  <a href="https://github.com/while-one/caffeine-hal/blob/main/LICENSE">
    <img src="https://img.shields.io/github/license/while-one/caffeine-hal?style=flat-square&color=blue" alt="License: MIT">
  </a>
</p>

# Caffeine-Template

This is the foundational template repository for all new components within the **Caffeine Framework** ecosystem (e.g., new hardware ports, middleware libraries, and application layers).

## Shared Ecosystem Standards

By instantiating a repository from this template, you inherit the framework's strict coding standards and CI infrastructure:

*   **Formatting (`config/.clang-format`):** Enforces a 120-column limit, 4-space indentation, and Allman-style braces.
*   **Static Analysis (`config/.clang-tidy`):** Enforces strict C11 compliance, memory safety rules (no dynamic allocation), and best practices for embedded systems.
*   **GitHub Infrastructure (`.github/`):** Pre-configured Pull Request templates, repository ownership rules, and baseline Continuous Integration workflows.

## Next Steps for New Components

1.  Update this `README.md` to describe the specific purpose of the new repository (e.g., "STM32 Porting Layer", "Modbus Middleware").
2.  Review and customize `.github/workflows/ci.yml` if your component requires a specific cross-compilation matrix (like ARM GCC) rather than native testing.
3.  Add the specific source files and CMake configuration (e.g., `CMakePresets.json`) as outlined in the [Caffeine-HAL Architecture Guide](https://github.com/while-one/caffeine-hal).
