#ifndef NF_TEST_SCREEN_H
#define NF_TEST_SCREEN_H

#include <pico/stdlib.h>
#include "pwm-tone.h"
#include "nf_memory.h"
#include "nf_menu/nf_menu_common.h"

#define SSR0_INDEX 0
#define SSR1_INDEX 1
#define FAN0_INDEX 2
#define FAN1_INDEX 3

#define ANIMATION_TIMEOUT 1000

#define MEMORY_SHOULD_CLEAR 1
#define MEMORY_CLEARED 2

typedef struct nf_test_menu_state {
    _nf_memory_state_t* _memory;
    uint cleaned_memory;
    uint cleared_animation;
    absolute_time_t animation_timeout;

    uint selected_option;
    bool testing;
    bool cancel;
    uint countdown_time;
    tonegenerator_t* _tonegen;
    struct repeating_timer *timer;
} _nf_test_menu_state_t;

void nf_test_render(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data);
void nf_test_btn_handler(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data);
void nf_test_menu_init(_nf_menu_t* _menu_state, _nf_memory_state_t* memory);


#endif