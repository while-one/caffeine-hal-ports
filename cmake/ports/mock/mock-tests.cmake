# This recipe is strictly for host-based mock testing.
# It does NOT compile a hardware port library.
# Instead, the test executables in tests/ will manually compile the specific 
# VMT sources (e.g., src/stm32f4/stm32f4_hal_uart.c) using the host compiler
# alongside mocked vendor headers.

# Create an empty interface to satisfy CMake target linkage in tests/CMakeLists.txt
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/mock>)
