# We need C++ for GoogleTest
enable_language(CXX)

option(FORCE_FETCH_DEPS "Force downloading dependencies even if found on system" OFF)

if(NOT FORCE_FETCH_DEPS)
    find_package(GTest QUIET)
endif()

if(NOT GTest_FOUND)
    message(STATUS "GTest not found locally, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
      DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    # Prevent GTest from overriding parent settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
else()
    message(STATUS "Using System-provided GoogleTest (Skip Download)")
endif()

# Alias GoogleTest targets with project prefix for better identification
add_library(${PROJECT_NAME}-gmock_main INTERFACE)
if(GTest_FOUND)
    target_link_libraries(${PROJECT_NAME}-gmock_main INTERFACE GTest::gmock_main)
else()
    target_link_libraries(${PROJECT_NAME}-gmock_main INTERFACE gmock_main)
endif()
# Gather all port tests
file(GLOB_RECURSE TEST_SOURCES "stm32/**/*.cpp" "posix/**/*.cpp" "gigadevice/**/*.cpp")

# Gather all port headers for IDE support (syntax highlighting)
file(GLOB_RECURSE PORT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/../src/**/*.h")

# We only create the test target if there are tests, to avoid CMake errors
if(TEST_SOURCES)
    # Note: To test hardware VMTs natively, we compile the source files directly
    # into the test executable here, rather than linking the cross-compiled port library.
    # e.g., list(APPEND TEST_SOURCES "../src/stm32/stm32f4/cfn_hal_uart.c")

    add_executable(${PROJECT_NAME}-test ${TEST_SOURCES} ${PORT_HEADERS})

    # Always enable coverage instrumentation for host-side unit tests
    target_compile_options(${PROJECT_NAME}-test PRIVATE --coverage -O0 -g)
    target_link_options(${PROJECT_NAME}-test PRIVATE --coverage)

    # Link the GoogleTest mock framework
    target_link_libraries(${PROJECT_NAME}-test
        PRIVATE
        caffeine::hal-ports # Empty interface in mock-tests preset
        ${PROJECT_NAME}-gmock_main
    )

    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME}-test)

    find_program(GCOVR_PATH gcovr)
    if(GCOVR_PATH)
        add_custom_target(${PROJECT_NAME}-coverage
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND ${GCOVR_PATH}
                --root ${CMAKE_SOURCE_DIR}
                --filter "${CMAKE_SOURCE_DIR}/src/"
                --exclude-unreachable-branches
                --gcov-ignore-errors=no_working_dir_found
                --print-summary
                --html-details coverage.html
                --txt coverage.txt
                ${CMAKE_BINARY_DIR}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS ${PROJECT_NAME}-test
            COMMENT "Running tests and generating coverage report (HTML and TXT)"
        )
    else()
        message(WARNING "gcovr not found! Coverage target will not be available.")
    endif()
endif()
