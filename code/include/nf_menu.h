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
#define REPEAT_FOR_EDIT_INCR 5

typedef struct nf_menu _nf_menu_t;
typedef struct nf_menu_option _nf_menu_option_t;

typedef struct nf_menu_option_fn_ptrs _nf_menu_option_fn_ptrs;

typedef struct nf_menu_option_fn_ptrs {
    void (*on_s)(_nf_menu_t* menu_state);
};

typedef struct nf_menu_option {
    void* extra_data;
    nf_menu_option_fn_ptrs fnptrs;
} _nf_menu_option_t;


/// USE PTR POINTERS as a menu mechanism!
/// you have a block of menu options use pointers for the forward and back ones
/// you know? it's smart! remember this brain moment

typedef void (*on_select)(void* _menu_state, bool repeat);

typedef struct {
    uint current_profile_option;
    uint error_counter;
    uint edit_repeat_count;
    uint edit_base_value;
    uint edit_index[2];
    bool edit_finished;
    absolute_time_t update_timeout;
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

typedef struct nf_menu {
    ssd1306_t* _disp_ptr;
    _nf_menu_option* menu_options;
    uint current_menu_option; 
    uint refresh_ms;
} _nf_menu_t;

void nf_menu_init(_nf_menu_t* _menu_state, ssd1306_t* _disp_ptr, nf_profile_t* _profiles);
void nf_menu_render(_nf_menu_t* _menu_state);


#endif