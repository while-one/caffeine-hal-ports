cmake_minimum_required(VERSION 3.21)
project(caffeine-hal-ports VERSION 0.1.0 LANGUAGES C ASM)

# Global standards for the Caffeine Framework
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# User-configurable flags (with sensible defaults)
option(CAFFEINE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
set(CAFFEINE_OPTIMIZATION_LEVEL "-Os" CACHE STRING "Compiler optimization level (e.g., -O0, -O2, -Os, -O3)")

# Universal strict warnings and preparation for dead-code elimination
set(CAFFEINE_COMPILE_OPTIONS
    -Wall -Wextra -Wpedantic -Wshadow
    -ffunction-sections -fdata-sections
    ${CAFFEINE_OPTIMIZATION_LEVEL}
)

if(CAFFEINE_WARNINGS_AS_ERRORS)
    list(APPEND CAFFEINE_COMPILE_OPTIONS -Werror)
endif()

# Useful for IDEs, clang-tidy, and clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Dependency: Caffeine-HAL ---
include(FetchContent)
FetchContent_Declare(
    caffeine-hal
    GIT_REPOSITORY https://github.com/while-one/caffeine-hal.git
    GIT_TAG        v0.1.2
)
FetchContent_MakeAvailable(caffeine-hal)

if(NOT DEFINED CAFFEINE_VENDOR)
    message(FATAL_ERROR "Please define CAFFEINE_VENDOR in your CMakePreset (e.g., stm32)")
endif()

if(NOT DEFINED CAFFEINE_PORT_FAMILY)
    message(FATAL_ERROR "Please configure using a CMakePreset (e.g., cmake --preset stm32f407-release)")
endif()

# Dynamically include the specific vendor recipe
include(cmake/ports/${CAFFEINE_VENDOR}/${CAFFEINE_PORT_FAMILY}.cmake)

# --- Universal Port Target Configuration ---
# Depending on the recipe (e.g., mock-tests vs stm32f4), the target might be an INTERFACE or STATIC library.
get_target_property(TARGET_TYPE ${PROJECT_NAME} TYPE)

if(TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
    target_compile_options(${PROJECT_NAME} INTERFACE ${CAFFEINE_COMPILE_OPTIONS})
    target_link_libraries(${PROJECT_NAME} INTERFACE caffeine::hal)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE ${CAFFEINE_COMPILE_OPTIONS})
    target_link_libraries(${PROJECT_NAME} PUBLIC caffeine::hal)
endif()

# Create the standard generic alias for user applications
add_library(caffeine::hal-ports ALIAS ${PROJECT_NAME})

# --- Unit Testing (CTest & GoogleTest) ---
option(CAFFEINE_PORT_BUILD_TESTS "Build the unit tests (Requires host compiler)" OFF)

if(CAFFEINE_PORT_BUILD_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()

# ==============================================================================
# Tooling: Code Formatting & Static Analysis
# ==============================================================================
# We use CONFIGURE_DEPENDS so CMake knows to re-run if we add or remove files.

file(GLOB_RECURSE PORT_SOURCES_C CONFIGURE_DEPENDS "src/stm32/**/*.c")

file(GLOB_RECURSE PORT_SOURCES_H CONFIGURE_DEPENDS "src/stm32/**/*.h")

file(GLOB_RECURSE TEST_SOURCES_CPP CONFIGURE_DEPENDS "tests/**/*.cpp")

file(GLOB_RECURSE TEST_SOURCES_H CONFIGURE_DEPENDS "tests/**/*.h")

set(ALL_FORMAT_SOURCES ${PORT_SOURCES_C} ${PORT_SOURCES_H} ${TEST_SOURCES_CPP} ${TEST_SOURCES_H})

set(PORT_SOURCES ${PORT_SOURCES_C}) # Only .c for clang-tidy

set(ALL_ANALYSIS_SOURCES ${PORT_SOURCES_C} ${TEST_SOURCES_CPP})

# 1. Code Formatting (clang-format)
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    option(FORMAT_DRY_RUN "Run clang-format in dry-run mode (useful for CI)" OFF)
    if(FORMAT_DRY_RUN)
        set(FORMAT_ARGS --dry-run --Werror)
        set(FORMAT_COMMENT "Checking formatting with clang-format (BARR-C style)...")
    else()
        set(FORMAT_ARGS -i)
        set(FORMAT_COMMENT "Formatting files with clang-format (BARR-C style)...")
    endif()

    add_custom_target(
        ${PROJECT_NAME}-format
        COMMAND ${CLANG_FORMAT}
        ${FORMAT_ARGS}
        -style=file:${PROJECT_SOURCE_DIR}/config/coding/.clang-format
        ${ALL_FORMAT_SOURCES}
        COMMENT "${FORMAT_COMMENT}"
    )
else()
    message(WARNING "clang-format not found. '${PROJECT_NAME}-format' target will not be available.")
endif()

# 2. Static Analysis (cppcheck)
find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    add_custom_target(
        ${PROJECT_NAME}-cppcheck
        COMMAND ${CPPCHECK}
        --project=${CMAKE_BINARY_DIR}/compile_commands.json
        --enable=all
        --suppress=missingIncludeSystem
        --suppress=unusedFunction
        --suppress=*:*/_deps/*
        --suppress=*:*/caffeine-hal/*
        --inline-suppr
        -D__GNUC__
        --file-filter=${PROJECT_SOURCE_DIR}/src/stm32/*
        COMMENT "Running cppcheck static analysis using compile_commands.json..."
    )
    if(NOT TARGET ${PROJECT_NAME}-analyze)
        add_custom_target(${PROJECT_NAME}-analyze COMMENT "Running full static analysis suite...")
    endif()
    add_dependencies(${PROJECT_NAME}-analyze ${PROJECT_NAME}-cppcheck)
else()
    message(WARNING "cppcheck not found. '${PROJECT_NAME}-cppcheck' target will not be available.")
endif()

# 3. Static Analysis (clang-tidy)
find_program(CLANG_TIDY clang-tidy)
if(CLANG_TIDY)
    add_custom_target(
        ${PROJECT_NAME}-tidy
        COMMAND ${CLANG_TIDY}
        -p=${CMAKE_BINARY_DIR}
        --config-file=${PROJECT_SOURCE_DIR}/config/coding/.clang-tidy
        --header-filter="${PROJECT_SOURCE_DIR}/src/stm32/.*/cfn_hal_.*"
        ${CAFFEINE_CLANG_TIDY_EXTRA_ARGS}
        ${PORT_SOURCES}
        COMMENT "Running clang-tidy static analysis using compile_commands.json..."
    )
    if(TARGET ${PROJECT_NAME}-analyze)
        add_dependencies(${PROJECT_NAME}-analyze ${PROJECT_NAME}-tidy)
    endif()
else()
    message(WARNING "clang-tidy not found. '${PROJECT_NAME}-tidy' target will not be available.")
endif()

# ==============================================================================
# Installation
# ==============================================================================
include(GNUInstallDirs)

# 1. Install the built static library target
install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}_Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# 2. Export the target to be used by other projects via find_package
install(
    EXPORT ${PROJECT_NAME}_Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE caffeine::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
)

# 3. Generate Version Configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 4. Generate the main Package Config
# We define a custom path for the linker script so the template can export it
set(PACKAGE_LINKER_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/linker")

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
    PATH_VARS PACKAGE_LINKER_INSTALL_DIR
)

# 5. Install the Config files
install(
    FILES
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
)

# 6. Install the generated linker script (if applicable for this port)
if(DEFINED CAFFEINE_BOARD_LINKER)
    # TODO: This placeholder will be replaced by the actual dynamic linker generator script.
    # We use file(GENERATE) to ensure the file is known to the build system before the install phase.
    file(GENERATE OUTPUT "${GENERATED_LINKER_SCRIPT}" CONTENT "/* Dynamic Linker Script Placeholder for ${CAFFEINE_BOARD_LINKER} */\n")

    install(
        FILES "${GENERATED_LINKER_SCRIPT}"
        DESTINATION ${PACKAGE_LINKER_INSTALL_DIR}
    )
endif()
