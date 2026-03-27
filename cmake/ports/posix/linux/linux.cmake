# This script defines the POSIX-compliant Linux port.
# Used for host-side execution and high-level Linux applications.

set(PORT_SOURCES_LIST
    ${PROJECT_SOURCE_DIR}/src/posix/linux/cfn_hal_uart.c
)

# Main target configuration is handled by the root CMakeLists.txt
# using the PORT_SOURCES_LIST variable populated here.
