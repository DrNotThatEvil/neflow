#ifndef NF_TEMPSYS_H
#define NF_TEMPSYS_H

#include <pico/multicore.h>

#include "nf_common.h"
#include "nf_max31855.h"


#define NORMAL_ALLOWED_CHANGERATE 3.0

typedef enum nf_tempsys_state {
    ERROR,
    PRE_INIT,
    POST_INIT,
    NORMAL,
    CALIBRATION
} _nf_tempsys_state_t;

typedef enum nf_tempsys_temp_mode {
    UNKNOWN,
    USE_TEMP0,
    USE_TEMP1,
    USE_BOTH
} _nf_tempsys_temp_mode_t;

typedef struct nf_temps {
    double internal;
    double thermocouple;
    double change_internal;
    double change_thermocouple;
} _nf_temps_t;

typedef struct nf_tempsys {
    _nf_max31855_t* _temp;
    _nf_tempsys_state_t _state;
    _nf_tempsys_temp_mode_t _tempmode;
    _nf_temps_t _results[2][2];
    uint read_index[2];
    uint write_index[2];
} _nf_tempsys_t;


void nf_initalize_tempsys(_nf_tempsys_t* _tempsys);
void nf_tempsys_loop(_nf_tempsys_t* _tempsys);

void _nf_tempsys_update_temps(_nf_tempsys_t* _tempsys, _nf_max31855_result_t* results);
void _nf_swap_indexes(_nf_tempsys_t* _tempsys);
void _nf_trigger_error(_nf_tempsys_t* _tempsys);


/// TODO: (2023-12-16, Tweak the NORMAL_ALLOWED_CHANGE rate, it's a bit to sensitive)
/// Allow the temp system access to the nf_state, so it can read the PID settings.
// After that you can try running it with a pid for a bit! YAY
// Almost ready to get it like bolted up!

#endif