set(NEFLOW_HEADER_LIST 
    "${neflow_SOURCE_DIR}/include/nf_common.h"
    "${neflow_SOURCE_DIR}/include/nf_memory.h"
    "${neflow_SOURCE_DIR}/include/nf_profile.h"
    "${neflow_SOURCE_DIR}/include/nf_config.h"
    "${neflow_SOURCE_DIR}/include/nf_menu.h"
    "${neflow_SOURCE_DIR}/include/nf_max31855.h"
    "${neflow_SOURCE_DIR}/include/nf_tempsys.h"
    "${neflow_SOURCE_DIR}/include/nf_menu/nf_menu_common.h"
    "${neflow_SOURCE_DIR}/include/nf_menu/nf_main_menu_screen.h"
    "${neflow_SOURCE_DIR}/include/nf_menu/nf_profiles_screen.h"
    "${neflow_SOURCE_DIR}/include/nf_menu/nf_profile_edit_screen.h"
    "${neflow_SOURCE_DIR}/include/nf_menu/nf_profile_run_screen.h"
    "${neflow_SOURCE_DIR}/include/nf_menu/nf_config_edit_screen.h"
    "${neflow_SOURCE_DIR}/include/nf_menu/nf_graph.h"
    "${neflow_SOURCE_DIR}/include/nf_menu/nf_graph_font.h"
    "${neflow_SOURCE_DIR}/include/nf_menu/nf_calibration_screen.h"
    "${neflow_SOURCE_DIR}/include/nf_menu/nf_test_menu_screen.h"
    "${neflow_SOURCE_DIR}/include/font.h"
    "${neflow_SOURCE_DIR}/include/ssd1306.h"
    "${neflow_SOURCE_DIR}/include/debounce.h"
)

add_executable(neflow
    main.c
    ssd1306.c
    debounce.c
    nf_memory.c
    nf_profile.c
    nf_profile.c
    nf_config.c
    nf_menu.c
    nf_max31855.c
    nf_tempsys.c
    nf_menu/nf_menu_common.c
    nf_menu/nf_main_menu_screen.c
    nf_menu/nf_profiles_screen.c
    nf_menu/nf_profile_edit_screen.c
    nf_menu/nf_profile_run_screen.c
    nf_menu/nf_config_edit_screen.c
    nf_menu/nf_calibration_screen.c
    nf_menu/nf_test_menu_screen.c
    ${REFLOW_PLATE_HEADER_LIST}
)

target_include_directories(neflow
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
)

target_compile_definitions(neflow PUBLIC ENABLE_TEST_MENU=1)

target_link_libraries(neflow pico_stdlib pico_multicore pico_malloc hardware_spi hardware_flash hardware_i2c pwm_tone)
target_link_options(neflow PUBLIC -Xlinker --print-memory-usage)

pico_add_extra_outputs(neflow)

pico_enable_stdio_usb(neflow 0)
pico_enable_stdio_uart(neflow 1)