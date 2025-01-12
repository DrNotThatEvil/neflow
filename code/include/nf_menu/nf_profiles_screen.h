#ifndef NF_PROFILES_SCREEN_H
#define NF_PROFILES_SCREEN_H

#include "nf_menu/nf_menu_common.h"
#include "nf_profile.h"

typedef struct nf_profiles_menu_state
{
    uint selected_profile;
    uint selected_option;
    bool selected;
    _nf_profile_state_t *_profile_state;
} _nf_profiles_menu_state_t;

void nf_profiles_render(_nf_menu_t *menu_state, ssd1306_t *disp_ptr,
                        void *extra_data);
void nf_profiles_btn_handler(_nf_menu_t *menu_state, uint btn, bool repeat,
                             void *extra_data);
void nf_profiles_menu_init(_nf_menu_t *_menu_state,
                           _nf_profile_state_t *_profile_state);

#endif
