#ifndef NF_PROFILE_EDIT_SCREEN_H
#define NF_PROFILE_EDIT_SCREEN_H

#include "nf_menu/nf_menu_common.h"
#include "nf_profile.h"

void nf_profile_edit_render(_nf_menu_t *menu_state, ssd1306_t *disp_ptr,
                            void *extra_data);
void nf_profile_edit_on_active_handler(_nf_menu_t *menu_state, void *extra_data,
                                       void *on_active_extra_data);
void nf_profile_edit_btn_handler(_nf_menu_t *menu_state, uint btn, bool repeat,
                                 void *extra_data);
void nf_profile_edit_menu_init(_nf_menu_t *_menu_state,
                               _nf_profile_state_t *_profile_state);

#endif
