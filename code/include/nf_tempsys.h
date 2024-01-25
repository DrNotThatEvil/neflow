#ifndef NF_TEMPSYS_H
#define NF_TEMPSYS_H

#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <hardware/structs/sio.h>

#include "nf_memory.h"
#include "nf_common.h"
#include "nf_max31855.h"

#define NORMAL_ALLOWED_CHANGERATE 5

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
    uint bad_reading_count;
} _nf_temps_t;

typedef struct nf_tempsys {
    _nf_memory_state_t* _memory;
    _nf_max31855_t* _temp;
    _nf_tempsys_state_t _prev_state;
    _nf_tempsys_state_t _curr_state;
    _nf_tempsys_temp_mode_t _tempmode;
    absolute_time_t _pid_timeout; 

    uint heater_state;
    float pid_data[3];
    float integral;
    float prev_error;
    float pid_output;

    _nf_temps_t _results[2][2];
    uint read_index[2];
    uint write_index[2];
} _nf_tempsys_t;


void nf_tempsys_init(_nf_tempsys_t* _tempsys, _nf_memory_state_t* _memory);
void nf_tempsys_set_state(_nf_tempsys_t* _tempsys, uint new_state);
void nf_tempsys_update(_nf_tempsys_t* _tempsys);

void _nf_tempsys_update_temps(_nf_tempsys_t* _tempsys, _nf_max31855_result_t* results);
void _nf_swap_indexes(_nf_tempsys_t* _tempsys);
void _nf_trigger_error(_nf_tempsys_t* _tempsys, uint error_flag);

void _nf_pid_controller(_nf_tempsys_t* _tempsys, float setpoint);

/// TODO: (2023-12-16, Tweak the NORMAL_ALLOWED_CHANGE rate, it's a bit to sensitive)
/// Allow the temp system access to the nf_state, so it can read the PID settings.
// After that you can try running it with a pid for a bit! YAY
// Almost ready to get it like bolted up!

// ALSO IMPORTANT, make sure you expand the error state to turn everything important off!
// Currently it's only the 'normal' state doing some sanity checking (temprature rise is weird when heaters are never supposed to be on)
// but in other states the heater will get turned on so it's VERY important to turn it off if ERROR.

#endif