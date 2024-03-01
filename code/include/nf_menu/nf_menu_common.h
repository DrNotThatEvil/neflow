#ifndef NF_MENU_COMMON_H
#define NF_MENU_COMMON_H

#include <pico/util/queue.h>
#include <pico/multicore.h>

#include "nf_common.h"
#include "nf_tempsys.h"
#include "pwm-tone.h"
#include "ssd1306.h"
#include "nf_memory.h"
#include "nf_tempsys.h"
#include "nf_profile.h"

#define MAIN_MENU_SCREEN_ID 0
#define PROFILES_SCREEN_ID 1
#define PROFILE_EDIT_SCREEN_ID 2
#define PROFILE_RUN_SCREEN_ID 3
#define CALIBRATION_SCREEN_ID 4
#define TEST_SCREEN_ID 5
#define CONFIG_SCREEN_ID 6

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

typedef struct nf_profile_run_screen_state {
    _nf_profile_t* current_profile;
    _nf_profile_state_t* _profile_state;
    bool _running;

    uint8_t _avg_index;
    uint8_t _sample_count;
    double _sum;
    double _averages[100];

    uint8_t _temp_high_value;
    uint16_t _sec_high_value;
    absolute_time_t _sample_timeout;
    absolute_time_t _avg_timeout;
    
} _nf_profile_run_screen_state_t;

typedef struct nf_menu_screen_fn_ptrs {
    void (*on_active)(_nf_menu_t* menu_state, void* extra_data, void* on_active_extra_data);
    void (*on_render)(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data);
    void (*on_btn)(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data);
} _nf_menu_screen_fn_ptrs_t;

typedef struct nf_menu_screen {
    uint id;
    void* extra_data;
    bool autoclear;
    _nf_menu_screen_fn_ptrs_t fnptrs;
    _nf_menu_screen_t* next;
} _nf_menu_screen_t;

typedef enum nf_menu_state {
    MENU_STATE_ERROR = 0,
    MENU_STATE_INIT,
    MENU_STATE_NORMAL,
    MENU_STATE_CALIBRATION,
    MENU_STATE_REFLOW
} _nf_menu_state_t;

typedef struct nf_menu_error_state {
    uint error_type;
    uint error_animation;
} _nf_menu_error_state_t;

typedef struct nf_button_state {
    bool pressed;
    bool held;
    uint held_count;
    bool released;
} nf_btn_state_t;

typedef struct nf_menu {
    absolute_time_t btn_update_timeout;
    absolute_time_t scr_update_timeout;
    queue_t tempsys_msg_q; 
    nf_btn_state_t _buttons[3];
    tonegenerator_t* _tonegen;
    ssd1306_t* _disp_ptr;
    _nf_tempsys_t* _tempsys;
    double cur_temp;
    uint heater_state;
    _nf_menu_state_t _state;
    _nf_menu_error_state_t _error_state;
    _nf_menu_screen_t* menu_screens;
    _nf_menu_screen_t** current_screen;
    uint refresh_ms;
} _nf_menu_t;


void draw_edit_item(ssd1306_t* disp_ptr, uint y, bool selected, const char* str);
void draw_edit_value(ssd1306_t* disp_ptr, uint y, bool selected, const char* str);

void nf_menu_add_screen(_nf_menu_screen_t** _menu_screens_ptr, _nf_menu_screen_t* _new_screen);
void nf_menu_change_screen_with_data(_nf_menu_t* _menu, uint screen_id, void* extra_data);
void nf_menu_change_screen(_nf_menu_t* _menu, uint screen_id);

void nf_menu_change_state(_nf_menu_t* _menu, _nf_menu_state_t state);
void nf_set_profile(_nf_menu_t* _menu, void* _profile);

void draw_prev_section(ssd1306_t* disp_ptr, const char* str);
void draw_next_arrow(ssd1306_t* disp_ptr);

#endif