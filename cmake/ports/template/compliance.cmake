# Gather all template C files
file(GLOB TEMPLATE_SOURCES "src/template/*.c")

# We build it as a static library to ensure everything compiles and links properly
add_library(${PROJECT_NAME} STATIC
    ${TEMPLATE_SOURCES}
)

# The template headers are local to the port
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/template
)
