#ifndef NF_CALIBRATE_SCREEN_H
#define NF_CALIBRATE_SCREEN_H

#include "nf_menu/nf_menu_common.h"

typedef struct nf_calibrate_menu_state {
    uint selected_profile;
    uint selected_option;
    bool selected;
} _nf_calibrate_menu_state_t;

void nf_calibrate_render(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data);
void nf_calibrate_btn_handler(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data);
void nf_calibrate_menu_init(_nf_menu_t* _menu_state, nf_profile_t* _profiles_ptr);


#endif