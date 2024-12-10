#ifndef NF_TEMPSYS_H
#define NF_TEMPSYS_H

#include <hardware/structs/sio.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <pico/util/queue.h>

#include "nf_common.h"
#include "nf_max31855.h"
#include "nf_memory.h"
#include "nf_profile.h"

#define NORMAL_ALLOWED_CHANGE_RATE 8
#define NORMAL_TO_LOW_TEMP         5
#define NORMAL_TO_HIGH_TEMP        50

#define CALIBRATION_TO_HIGH_TEMP        150
#define CALIBRATION_ALLOWED_CHANGE_RATE 15

#define RUNNING_TO_HIGH_TEMP        210
#define RUNNING_ALLOWED_CHANGE_RATE 15

typedef enum nf_tempsys_state
{
    ERROR,
    PRE_INIT,
    POST_INIT,
    NORMAL,
    CALIBRATION,
    RUNNING,
    FINISHED
} _nf_tempsys_state_t;

typedef enum nf_tempsys_temp_mode
{
    UNKNOWN,
    USE_TEMP0,
    USE_TEMP1,
    USE_BOTH
} _nf_tempsys_temp_mode_t;

typedef struct nf_temps
{
    double internal;
    double thermocouple;
    double change_internal;
    double change_thermocouple;
    uint bad_reading_count;
} _nf_temps_t;

typedef struct nf_thread_msg
{
    uint8_t msg_type;
    uint16_t simple_msg_value;
    void *value_ptr;
} _nf_thread_msg;

typedef struct nf_tempsys
{
    _nf_memory_state_t *_memory;
    _nf_max31855_t *_temp;
    queue_t *_menu_msq_queue_ptr;
    _nf_tempsys_state_t _prev_state;
    _nf_tempsys_state_t _curr_state;
    _nf_tempsys_temp_mode_t _tempmode;
    absolute_time_t _pid_timeout;

    uint8_t _cur_stage_index;
    uint16_t _cur_stage_l_cnt;
    _nf_profile_t *_profile;

    uint heater_state;
    float pid_data[3];
    float integral;
    float prev_error;
    float pid_output;

    bool hit_target;

    double bootup_temp;
    // Result
    _nf_temps_t _results[TEMPSYS_RESULT_INDEX_MAX][TEMPSYS_RESULT_INDEX_MAX];
    uint read_index[TEMPSYS_RESULT_INDEX_MAX];
    uint write_index[TEMPSYS_RESULT_INDEX_MAX];
} _nf_tempsys_t;

void nf_tempsys_init(_nf_tempsys_t *_tempsys, _nf_memory_state_t *_memory);
void nf_tempsys_update(_nf_tempsys_t *_tempsys);
void nf_tempsys_set_menu_queue(_nf_tempsys_t *_tempsys,
                               queue_t *_menu_msg_queue_ptr);

void _nf_sanity_check(_nf_tempsys_t *_tempsys);
void _nf_tempsys_set_state(_nf_tempsys_t *_tempsys,
                           _nf_tempsys_state_t new_state);
void _nf_tempsys_update_temps(_nf_tempsys_t *_tempsys,
                              _nf_max31855_result_t *results);
void _nf_tempsys_update_avg_temp(_nf_tempsys_t *_tempsys);

void _nf_tempsys_handle_thread_messages(_nf_tempsys_t *_tempsys);
void _nf_swap_indexes(_nf_tempsys_t *_tempsys);
void _nf_send_temp_initialized(_nf_tempsys_t *_tempsys);
void _nf_send_temp_update(_nf_tempsys_t *_tempsys);
void _nf_send_finished(_nf_tempsys_t *_tempsys);
void _nf_trigger_error(_nf_tempsys_t *_tempsys, uint error_flag);

float _nf_calculate_target_temp(_nf_tempsys_t *_tempsys);

void _nf_pid_controller(_nf_tempsys_t *_tempsys, float setpoint);

#endif
