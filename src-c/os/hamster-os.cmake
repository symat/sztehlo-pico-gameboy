add_library(hamster-os INTERFACE)

target_sources(hamster-os INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/kernel.c
)

# Add include directory
target_include_directories(hamster-os INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(hamster-os INTERFACE
#    pico_multicore
#    hardware_dma
#    hardware_pio
#    hardware_timer
    hamster-libs
    pico_stdlib
)