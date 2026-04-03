cmake_minimum_required(VERSION 3.23)
project(caffeine-hal-ports VERSION 0.3.4 LANGUAGES C CXX ASM)
# 3. Code Quality (Formatting & Analysis)
# Define the absolute universe of sources for this repository
file(GLOB_RECURSE ALL_UNIVERSE_SOURCES 
    "${PROJECT_SOURCE_DIR}/src/*.c"
    "${PROJECT_SOURCE_DIR}/src/*.h"
    "${PROJECT_SOURCE_DIR}/tests/*.cpp"
    "${PROJECT_SOURCE_DIR}/tests/*.c"
    "${PROJECT_SOURCE_DIR}/tests/*.h"
)

include(${PROJECT_SOURCE_DIR}/caffeine-build/cmake/CaffeineMacros.cmake)
cfn_add_universe_targets(INPUTS ${ALL_UNIVERSE_SOURCES})

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
    GIT_TAG        v0.5.6
)

FetchContent_MakeAvailable(caffeine-hal)

# --- Modular Port Selection ---
# Each port is isolated in its own directory. 
# We define CAFFEINE_HAL_PORT_VENDOR and CFN_HAL_PORT_FAMILY in the CMakePresets.json
if(NOT DEFINED CFN_HAL_PORT_VENDOR OR NOT DEFINED CFN_HAL_PORT_FAMILY)
    message(FATAL_ERROR "Please define CFN_HAL_PORT_VENDOR and CFN_HAL_PORT_FAMILY in your CMakePreset (e.g., stm32/stm32f4)")
endif()

# Map the family to a subdirectory
set(PORT_FAMILY_DIR "cmake/ports/${CFN_HAL_PORT_VENDOR}/${CFN_HAL_PORT_FAMILY}")
set(PORT_BASE_CMAKE "${PORT_FAMILY_DIR}/${CFN_HAL_PORT_FAMILY}.cmake")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PORT_BASE_CMAKE}")
    # Initialize port source list
    set(PORT_SOURCES_LIST "")

    # Include the base family logic (Modules and Vendor SDK)
    include("${PORT_BASE_CMAKE}")
    set(ANALYSIS_SUBDIR "${CFN_HAL_PORT_VENDOR}/${CFN_HAL_PORT_FAMILY}")
    
    # --- Universal Port Target Configuration ---
    add_library(${PROJECT_NAME} STATIC ${PORT_SOURCES_LIST})

    # Apply hardware-specific configuration (CPU flags, includes, linker scripts)
    if(COMMAND cfn_port_apply_target_config)
        cfn_port_apply_target_config(${PROJECT_NAME})
    else()
        # Fallback for generic ports (Template, POSIX)
        target_link_libraries(${PROJECT_NAME} PUBLIC caffeine::hal)
        if(CFN_BUILD_TESTS)
            target_link_libraries(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:caffeine::hal-mock>)
        endif()
        target_include_directories(${PROJECT_NAME} PUBLIC 
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/${ANALYSIS_SUBDIR}>
        )
    endif()

    add_library(caffeine::hal-ports ALIAS ${PROJECT_NAME})
else()
    message(FATAL_ERROR "Port implementation not found: ${PORT_BASE_CMAKE}")
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

cfn_add_code_quality_targets(
    ${PROJECT_NAME}
    ANALYSIS_SOURCES ${PORT_SOURCES_LIST}
    CPPCHECK_FILE_FILTER "${PROJECT_SOURCE_DIR}/src/${ANALYSIS_SUBDIR}/*"
    TIDY_HEADER_FILTER "${PROJECT_SOURCE_DIR}/src/${ANALYSIS_SUBDIR}/.*"
    CPPCHECK_ARGS --suppress=missingInclude
)

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

# 7. Doxygen Documentation
cfn_add_docs(
    ${PROJECT_NAME}-docs
    INPUTS ${CFN_UNIVERSE_SOURCES} ${PROJECT_SOURCE_DIR}/README.md
    COMMENT "Generating API documentation with Doxygen"
)
