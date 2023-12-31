#ifndef nf_config_h
#define nf_config_h

#include <pico/stdlib.h>
#include "ssd1306.h"
#include "nf_profile.h"
#include "nf_menu.h"


#define NF_BTN0_PIN 14
#define NF_BTN1_PIN 15
#define NF_BTN2_PIN 13
//#define NF_BTN3_PIN 7

#define NF_MENU_BACK_BTN NF_BTN0_PIN
#define NF_MENU_NEXT_BTN NF_BTN1_PIN
#define NF_MENU_SELECT_BTN NF_BTN2_PIN
//#define NF_MENU_BACK_BTN NF_BTN3_PIN

#define NF_BACK_BTN_INDEX 0
#define NF_NEXT_BTN_INDEX 1
#define NF_SELECT_BTN_INDEX 2

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
    nf_menu_state_t _menu_state; /// TODO move this to the menu.h?
    nf_reflow_state_t _reflow_state; 
    ssd1306_t* _disp_ptr;
    nf_profile_t* _profiles;
    _nf_menu_t* _menu;
} nf_state_t;

void nf_init(nf_state_t* _nf_state);


#endif