add_library(sztehlo-libs INTERFACE)

target_sources(sztehlo-libs INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/display.c
)

# Add include directory
target_include_directories(sztehlo-libs INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(sztehlo-libs INTERFACE
#    pico_multicore
#    hardware_dma
#    hardware_pio
#    hardware_timer
    pico_stdlib
)