add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/posix/linux/cfn_hal_uart.c
)
# Export port-specific headers
target_include_directories(${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/posix/linux>
)
# Linux requires pthread for async operations
target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
