#ifndef NF_MAIN_MENU_H
#define NF_MAIN_MENU_H

#include "nf_menu/nf_menu_common.h"

#define PROFILES_INDEX 0
#define CALIBRATION_INDEX 1
#define CONFIG_INDEX 2

void nf_main_menu_render(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data);
void nf_main_menu_btn_handler(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data);
void nf_main_menu_init(_nf_menu_t* _menu_state);


#endif