#ifndef nf_config_h
#define nf_config_h

#include <pico/stdlib.h>

#include "nf_common.h"

#include "nf_memory.h"
#include "nf_menu.h"
#include "nf_profile.h"
#include "nf_tempsys.h"
#include "pwm-tone.h"
#include "ssd1306.h"

// State of the reflow oven.
typedef enum
{
    NONE,
    PREHEAT,
    SOAK,
    REFLOW,
    COOLDOWN
} nf_reflow_state_t;

typedef struct
{
    tonegenerator_t *_tonegen;
    nf_reflow_state_t _reflow_state;
    _nf_profile_state_t *_profile_state;
    _nf_tempsys_t *_tempsys;
    _nf_menu_t *_menu;
    _nf_memory_state_t *_memory;
    absolute_time_t _temploop_timer;
} nf_state_t;

void nf_init(nf_state_t *_nf_state);
void nf_tempsys_loop(nf_state_t *_nf_state);

#endif
