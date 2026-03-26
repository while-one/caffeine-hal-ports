cmake_minimum_required(VERSION 3.21)
project(caffeine-hal-ports VERSION 0.2.15 LANGUAGES C ASM)
include(${PROJECT_SOURCE_DIR}/caffeine-build/cmake/CaffeineMacros.cmake)

# Global standards for the Caffeine Framework
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

MESSAGE(STATUS "Target CAFFEINE_BUILD_STAGE: ${CAFFEINE_BUILD_STAGE}")

# Export compile commands for analysis tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Dependency: Caffeine-HAL ---
include(FetchContent)
FetchContent_Declare(
    caffeine-hal
    GIT_REPOSITORY https://github.com/while-one/caffeine-hal.git
    GIT_TAG        v0.4.15
)

FetchContent_MakeAvailable(caffeine-hal)

# --- Modular Port Selection ---
# Each port is isolated in its own directory. 
# We define CAFFEINE_HAL_PORT_VENDOR and CFN_HAL_PORT_FAMILY in the CMakePresets.json
if(NOT DEFINED CFN_HAL_PORT_VENDOR OR NOT DEFINED CFN_HAL_PORT_FAMILY)
    if(CFN_BUILD_TESTS)
        message(STATUS "No hardware port selected. Analyzing Template directory and linking against mock.")
        file(GLOB_RECURSE TEMPLATE_SOURCES "src/template/*.c")
        add_library(${PROJECT_NAME} STATIC ${TEMPLATE_SOURCES})
        target_link_libraries(${PROJECT_NAME} PUBLIC caffeine::hal)
        target_link_libraries(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:caffeine::hal-mock>)
        add_library(caffeine::hal-ports ALIAS ${PROJECT_NAME})
        set(ANALYSIS_SUBDIR "template") # Default to template for compliance checks
    else()
        message(FATAL_ERROR "Please define CFN_HAL_PORT_VENDOR and CFN_HAL_PORT_FAMILY in your CMakePreset (e.g., stm32/stm32f4)")
    endif()
else()
    # Map the family to a subdirectory
    set(PORT_FAMILY_DIR "cmake/ports/${CFN_HAL_PORT_VENDOR}/${CFN_HAL_PORT_FAMILY}")
    set(PORT_BASE_CMAKE "${PORT_FAMILY_DIR}/${CFN_HAL_PORT_FAMILY}.cmake")

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PORT_BASE_CMAKE}")
        # 1. First include the target-specific definitions if provided
        if(DEFINED CFN_HAL_PORT_TARGET)
            set(PORT_TARGET_CMAKE "${PORT_FAMILY_DIR}/${CFN_HAL_PORT_TARGET}.cmake")
            if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PORT_TARGET_CMAKE}")
                include("${PORT_TARGET_CMAKE}")
                message(STATUS "Loaded hardware target: ${CFN_HAL_PORT_TARGET}")
            else()
                message(FATAL_ERROR "Target definition not found: ${PORT_TARGET_CMAKE}")
            endif()
        endif()

        # 2. Then include the base family logic
        include("${PORT_BASE_CMAKE}")
        set(ANALYSIS_SUBDIR "${CFN_HAL_PORT_VENDOR}/${CFN_HAL_PORT_FAMILY}")
        
        # --- Universal Port Target Configuration ---
        get_target_property(TARGET_TYPE ${PROJECT_NAME} TYPE)

        if(TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
            target_compile_options(${PROJECT_NAME} INTERFACE ${CAFFEINE_COMPILE_OPTIONS})
            target_link_libraries(${PROJECT_NAME} INTERFACE caffeine::hal)
        else()
            target_compile_options(${PROJECT_NAME} PRIVATE ${CAFFEINE_COMPILE_OPTIONS})
            target_compile_options(${PROJECT_NAME} INTERFACE ${CAFFEINE_COMPILE_OPTIONS})
            target_link_libraries(${PROJECT_NAME} PUBLIC caffeine::hal)
        endif()

        # If building tests, we also need the mock library for dependencies
        if(CFN_BUILD_TESTS)
            target_link_libraries(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:caffeine::hal-mock>)
        endif()

        # Apply direct IRQ delegation definitions if requested by the application
        cfn_apply_direct_irq(${PROJECT_NAME})

        add_library(caffeine::hal-ports ALIAS ${PROJECT_NAME})
    else()
        message(FATAL_ERROR "Port implementation not found: ${PORT_BASE_CMAKE}")
    endif()
endif()

# Consistent naming for exported target
set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME hal-ports)

# --- Unit Testing (CTest & GoogleTest) ---
option(CFN_BUILD_TESTS "Build the unit tests (Requires host compiler)" OFF)
if(PROJECT_IS_TOP_LEVEL AND CFN_BUILD_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()

# ==============================================================================
# Tooling: Code Formatting & Static Analysis
# ==============================================================================

# Gather all source files for analysis (Port-specific only, ignoring tests)
# We glob based on the family directory to avoid noise from other ports
file(GLOB_RECURSE PORT_SOURCES_C "src/${ANALYSIS_SUBDIR}/*.c")
file(GLOB_RECURSE PORT_SOURCES_H "src/${ANALYSIS_SUBDIR}/*.h")

set(ALL_FORMAT_SOURCES ${PORT_SOURCES_C} ${PORT_SOURCES_H})
set(PORT_SOURCES ${PORT_SOURCES_C}) # Only .c for clang-tidy
set(ALL_ANALYSIS_SOURCES ${PORT_SOURCES_C})

# 1. Code Formatting (clang-format)
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT AND ALL_FORMAT_SOURCES)
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
        -style=file:${PROJECT_SOURCE_DIR}/caffeine-build/config/coding/.clang-format
        ${ALL_FORMAT_SOURCES}
        COMMENT "${FORMAT_COMMENT}"
    )
endif()

# 2. Static Analysis (cppcheck)
find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    add_custom_target(
        ${PROJECT_NAME}-cppcheck
        COMMAND ${CPPCHECK}
        ${CFN_CPPCHECK_FLAGS}
        --project=${CMAKE_BINARY_DIR}/compile_commands.json
        --file-filter=${PROJECT_SOURCE_DIR}/src/${ANALYSIS_SUBDIR}/*
        COMMENT "Running cppcheck static analysis using compile_commands.json..."
    )
    if(NOT TARGET ${PROJECT_NAME}-analyze)
        add_custom_target(${PROJECT_NAME}-analyze COMMENT "Running full static analysis suite...")
    endif()
    add_dependencies(${PROJECT_NAME}-analyze ${PROJECT_NAME}-cppcheck)
endif()

# 3. Static Analysis (clang-tidy)
find_program(CLANG_TIDY clang-tidy)
if(CLANG_TIDY)
    cfn_get_clang_tidy_extra_args(CLANG_TIDY_EXTRA_ARGS)

    add_custom_target(
        ${PROJECT_NAME}-tidy
        COMMAND ${CLANG_TIDY}
        -p=${CMAKE_BINARY_DIR}
        --config-file=${PROJECT_SOURCE_DIR}/caffeine-build/config/coding/.clang-tidy
        --header-filter="${PROJECT_SOURCE_DIR}/src/${ANALYSIS_SUBDIR}/.*"
        ${CLANG_TIDY_EXTRA_ARGS}
        ${ALL_ANALYSIS_SOURCES}
        COMMENT "Running clang-tidy static analysis using compile_commands.json..."
    )
    
    if(TARGET ${PROJECT_NAME}-analyze)
        add_dependencies(${PROJECT_NAME}-analyze ${PROJECT_NAME}-tidy)
    endif()
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

# 6. Install the actual linker scripts
file(GLOB ALL_LINKER_SCRIPTS "${PROJECT_SOURCE_DIR}/linker/*.ld")
if(ALL_LINKER_SCRIPTS)
    install(
        FILES ${ALL_LINKER_SCRIPTS}
        DESTINATION ${PACKAGE_LINKER_INSTALL_DIR}
    )
endif()