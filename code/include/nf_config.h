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
    nf_menu_state_t _menu_state;
    nf_reflow_state_t _reflow_state; 
    ssd1306_t* _disp_ptr;
    nf_profile_t* _profiles;
    _nf_menu_t* _menu;
} nf_state_t;

void nf_init(nf_state_t* _nf_state);


#endif