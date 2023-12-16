#ifndef NF_MENU_COMMON_H
#define NF_MENU_COMMON_H

#include "nf_common.h"
#include "nf_tempsys.h"
#include "pwm-tone.h"
#include "ssd1306.h"
#include "nf_memory.h"
#include "nf_profile.h"

#define MAIN_MENU_SCREEN_ID 0
#define PROFILES_SCREEN_ID 1
#define PROFILE_EDIT_SCREEN_ID 2
#define CALIBRATION_SCREEN_ID 3
#define TEST_SCREEN_ID 4
#define CONFIG_SCREEN_ID 5

typedef struct nf_menu _nf_menu_t;
typedef struct nf_menu_screen _nf_menu_screen_t;

typedef struct nf_menu_screen_fn_ptrs _nf_menu_screen_fn_ptrs_t;

typedef struct nf_profile_edit_screen_state {
    _nf_profile_t* current_profile;
    _nf_profile_state_t* _profile_state;
    //_nf_memory_state_t* _memory;
    uint selected_value;
    uint editing; 
    bool adjust;
} _nf_profile_edit_screen_state_t;

typedef struct nf_menu_screen_fn_ptrs {
    void (*on_active)(_nf_menu_t* menu_state, void* extra_data, void* on_active_extra_data);
    void (*on_render)(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data);
    void (*on_btn)(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data);
} _nf_menu_screen_fn_ptrs_t;

typedef struct nf_menu_screen {
    uint id;
    void* extra_data;
    _nf_menu_screen_fn_ptrs_t fnptrs;
    _nf_menu_screen_t* next;
} _nf_menu_screen_t;

typedef struct nf_menu {
    tonegenerator_t* _tonegen;
    ssd1306_t* _disp_ptr;
    _nf_menu_screen_t* menu_screens;
    _nf_menu_screen_t** current_screen;
    _nf_temps_t* _temps0;
    _nf_temps_t* _temps1;
    uint refresh_ms;
} _nf_menu_t;



void draw_edit_item(ssd1306_t* disp_ptr, uint y, bool selected, const char* str);
void draw_edit_value(ssd1306_t* disp_ptr, uint y, bool selected, const char* str);

void nf_menu_add_screen(_nf_menu_screen_t** _menu_screens_ptr, _nf_menu_screen_t* _new_screen);
void nf_menu_change_screen_with_data(_nf_menu_t* _menu, uint screen_id, void* extra_data);
void nf_menu_change_screen(_nf_menu_t* _menu, uint screen_id);

void draw_prev_section(ssd1306_t* disp_ptr, const char* str);
void draw_next_arrow(ssd1306_t* disp_ptr);

#endif