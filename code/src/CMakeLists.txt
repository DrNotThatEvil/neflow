set(NEFLOW_HEADER_LIST 
    "${neflow_SOURCE_DIR}/include/font.h"
    "${neflow_SOURCE_DIR}/include/ssd1306.h"
    "${neflow_SOURCE_DIR}/include/debounce.h"
)

add_executable(neflow
    main.c
    ssd1306.c
    debounce.c
    ${REFLOW_PLATE_HEADER_LIST}
)

target_include_directories(neflow
    PUBLIC ../include
)


target_link_libraries(neflow pico_stdlib hardware_i2c)
target_link_options(neflow PUBLIC -Xlinker --print-memory-usage)

pico_add_extra_outputs(neflow)

pico_enable_stdio_usb(neflow 0)
pico_enable_stdio_uart(neflow 1)