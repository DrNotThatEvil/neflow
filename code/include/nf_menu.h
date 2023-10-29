#ifndef NF_MENU_H
#define NF_MENU_H

#include <string.h>
#include <pico/stdlib.h>
#include <pico/types.h>
#include "nf_profile.h"
#include "ssd1306.h"

#define NOPTIONS 6
#define MAIN_OPTIONS 3

/// INDEXES
#define PROFILES 0


typedef void (*on_select)(void* _menu_state);

typedef struct {
    uint current_profile_option;
    uint error_counter;
    uint edit_index[2];
    nf_profile_t* _profiles;
} _nf_menu_profile;


typedef struct {
    char* name;
    on_select render_fn;
    on_select back_fn;
    on_select select_fn;
    on_select next_fn;
    void* extra_data;
} _nf_menu_option;

typedef struct {
    ssd1306_t* _disp_ptr;
    _nf_menu_option* menu_options;
    uint current_menu_option; 
} _nf_menu_t;

void nf_menu_init(_nf_menu_t* _menu_state, ssd1306_t* _disp_ptr, nf_profile_t* _profiles);
void nf_menu_render(_nf_menu_t* _menu_state);


#endif