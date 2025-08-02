add_executable(shell shell.c)

#pico_set_program_name(shell "shell")
#pico_set_program_version(shell "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(shell 0)
pico_enable_stdio_usb(shell 1)
#target_compile_definitions(shell PRIVATE
#    PICO_DEFAULT_UART_RX_PIN=17
#    PICO_DEFAULT_UART_TX_PIN=16
#)

#set_target_properties(shell PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
#)

# Add the standard library to the build
target_link_libraries(shell
        pico_stdlib
        sztehlo-libs)

# Add the standard include files to the build
#target_include_directories(shell PUBLIC
#        ${CMAKE_CURRENT_LIST_DIR}
#        ${CMAKE_CURRENT_LIST_DIR}/..
#        ${CMAKE_CURRENT_LIST_DIR}/../lib
#)

# create map/bin/hex file etc.
pico_add_extra_outputs(shell)

#target_link_options(shell 
#    PRIVATE "LINKER:--print-memory-usage"
#)