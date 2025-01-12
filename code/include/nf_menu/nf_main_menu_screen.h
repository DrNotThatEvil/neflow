#ifndef NF_MAIN_MENU_H
#define NF_MAIN_MENU_H

#include "nf_menu/nf_menu_common.h"

typedef struct nf_main_menu_state
{
    uint selected;
    uint line_offset;
    uint line_m;
} _nf_main_menu_state_t;

#define PROFILES_INDEX    0
#define CALIBRATION_INDEX 1
#define CONFIG_INDEX      2

#if ENABLE_TEST_MENU == 1
#define TEST_MENU_INDEX            3
#define MAX_MAIN_MENU_OPTION_INDEX (TEST_MENU_INDEX + 1)
#else
#define MAX_MAIN_MENU_OPTION_INDEX (CONFIG_INDEX + 1)
#endif

void nf_main_menu_render(_nf_menu_t *menu_state, ssd1306_t *disp_ptr,
                         void *extra_data);
void nf_main_menu_btn_handler(_nf_menu_t *menu_state, uint btn, bool repeat,
                              void *extra_data);
void nf_main_menu_init(_nf_menu_t *_menu_state);

#endif
