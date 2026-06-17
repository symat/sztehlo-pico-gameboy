add_library(hamster-libs INTERFACE)

target_sources(hamster-libs INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/display.c
)

# Add include directory
target_include_directories(hamster-libs INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(hamster-libs INTERFACE
#    pico_multicore
#    hardware_dma
#    hardware_pio
#    hardware_timer
    pico_stdlib
)