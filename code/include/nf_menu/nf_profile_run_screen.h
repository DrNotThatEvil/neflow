#ifndef NF_PROFILE_RUN_SCREEN_H
#define NF_PROFILE_RUN_SCREEN_H

#include "nf_profile.h"
#include "nf_menu/nf_menu_common.h"


void nf_profile_run_render(_nf_menu_t* _menu_state, ssd1306_t* disp_ptr, void* extra_data);
void nf_profile_run_on_active_handler(_nf_menu_t* _menu_state, void* extra_data, void* on_active_extra_data);
void nf_profile_run_btn_handler(_nf_menu_t* _menu_state, uint btn, bool repeat, void* extra_extra);
void nf_profile_run_screen_init(_nf_menu_t* _menu_state, _nf_profile_state_t* _profile_state);

#endif