#ifndef nf_config_h
#define nf_config_h

#include <pico/stdlib.h>

#include "nf_common.h"

#include "pwm-tone.h"
#include "ssd1306.h"
#include "nf_memory.h"
#include "nf_profile.h"
#include "nf_menu.h"

// State of the reflow oven.
typedef enum {
    NORMAL,
    CALIBATION,
    REFLOWING
} nf_menu_state_t;

typedef enum {
    NONE,
    PREHEAT,
    SOAK,
    REFLOW,
    COOLDOWN
} nf_reflow_state_t;

typedef struct {
    bool pressed;
    bool held;
    uint held_count;
    bool released;
} nf_btn_state;

typedef struct {
    nf_btn_state _buttons[3];
    tonegenerator_t* _tonegen;
    nf_menu_state_t _menu_state; /// TODO move this to the menu.h?
    nf_reflow_state_t _reflow_state; 
    ssd1306_t* _disp_ptr;
    nf_profile_t* _profiles;
    _nf_menu_t* _menu;
    _nf_memory_state_t* _memory;
} nf_state_t;

void nf_init(nf_state_t* _nf_state);


#endif