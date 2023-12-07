#ifndef NF_MENU_H
#define NF_MENU_H

#include <string.h>
#include <pico/stdlib.h>
#include <pico/types.h>

#include "pwm-tone.h"
#include "nf_menu/nf_menu_common.h"
#include "nf_menu/nf_main_menu_screen.h"
#include "nf_menu/nf_test_menu_screen.h"
#include "nf_menu/nf_config_edit_screen.h"
#include "nf_menu/nf_profiles_screen.h"
#include "nf_menu/nf_profile_edit_screen.h"


/*

#define NOPTIONS 6
#define MAIN_OPTIONS 3

/// INDEXES
#define PROFILES 0
#define REPEAT_FOR_EDIT_INCR 5

typedef struct nf_menu _nf_menu_t;
typedef struct nf_menu_screen _nf_menu_screen_t;
typedef struct nf_menu_screen_fn_ptrs _nf_menu_screen_fn_ptrs_t;

typedef struct nf_menu_screen_fn_ptrs {
    void (*on_render)(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data);
    void (*on_btn)(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data);
} _nf_menu_screen_fn_ptrs_t;

typedef struct nf_menu_screen {
    uint id;
    void* extra_data;
    _nf_menu_screen_fn_ptrs_t fnptrs;
    _nf_menu_screen_t* next;
} _nf_menu_screen_t;


/// USE PTR POINTERS as a menu mechanism!
/// you have a block of menu options use pointers for the forward and back ones
/// you know? it's smart! remember this brain moment

// Notes
// Split up screens in seperate files,
// Each screen gets a init functions
// Also aproach it diffently it's not a list of menu options
// its a list of screens, the main_menu just handles the display change in the renderer


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
    _nf_menu_screen_t* menu_screens;
    _nf_menu_screen_t** current_screen;
    uint refresh_ms;
} _nf_menu_t;

*/

void nf_menu_init(_nf_menu_t* _menu_state, ssd1306_t* _disp_ptr, tonegenerator_t* _tonegen, _nf_profile_state_t* _profile_state, _nf_memory_state_t* _memory);
void nf_menu_render(_nf_menu_t* _menu_state);
void nf_menu_btn_handler(_nf_menu_t* _menu_state, uint btn, bool repeat);


#endif