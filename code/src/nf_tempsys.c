#include "nf_tempsys.h"

void nf_tempsys_init(_nf_tempsys_t* _tempsys, _nf_memory_state_t* _memory)
{
    _tempsys->_memory = _memory;
    _tempsys->_temp  = malloc(sizeof(_nf_max31855_t));

    _tempsys->_menu_msq_queue_ptr = ((void*)0);
    
    nf_max31855_init(_tempsys->_temp);

    _tempsys->_curr_state = PRE_INIT;
    _tempsys->_prev_state = PRE_INIT;
    _tempsys->_tempmode = UNKNOWN;
    _tempsys->heater_state = 0;

    for(uint i = 0; i < 2; i++) {
        for(uint j = 0; j < 2; j++) {
            _nf_temps_t empty_temp = { .internal = 0.0, .thermocouple = 0.0, .change_internal = 0.0, .change_thermocouple = 0.0 };
            _tempsys->_results[i][j] = empty_temp;
        }
    }

    _tempsys->pid_data[0] = 25.f;
    _tempsys->pid_data[1] = 0.3f;
    _tempsys->pid_data[2] = 0.05f;
    _tempsys->integral = 0.f;
    _tempsys->prev_error = 0.f;
    _tempsys->pid_output = 0.f;

    _tempsys->read_index[0] = 0;
    _tempsys->read_index[1] = 0;
    
    _tempsys->write_index[0] = 1;
    _tempsys->write_index[1] = 1;
}

void nf_tempsys_update(_nf_tempsys_t* _tempsys)
{
    // Initaization takes 2 loops, one to determine tempmode and read frist temp.
    // Second one does the same but also gets correct change from previous reading.
    // else first reading would be a verry high change rate.
    if (_tempsys->_curr_state == PRE_INIT || _tempsys->_curr_state == POST_INIT) {
        // Determine temprature mode
        _nf_max31855_result_t temp0_result;
        nf_max31855_read(_tempsys->_temp, NF_TEMP0_CS, &temp0_result);
        _nf_tempsys_update_temps(_tempsys, &temp0_result);

        _nf_max31855_result_t temp1_result;
        nf_max31855_read(_tempsys->_temp, NF_TEMP1_CS, &temp1_result);
        _nf_tempsys_update_temps(_tempsys, &temp1_result);

        if(temp0_result.faults == 0 && temp1_result.faults == 0) {
            _tempsys->_tempmode = USE_BOTH;
        } else if (temp0_result.faults == 0 && temp1_result.faults > 0) {
            _tempsys->_tempmode = USE_TEMP0;
        } else if (temp0_result.faults > 0 && temp1_result.faults == 0) {
            _tempsys->_tempmode = USE_TEMP1;
        } else {
            _tempsys->_curr_state = ERROR;
            return;
        }

        _nf_swap_indexes(_tempsys);

        if (_tempsys->_curr_state == PRE_INIT) {
            _tempsys->_curr_state = POST_INIT;
            _tempsys->_prev_state = POST_INIT;
            return;
        } else {
            _tempsys->_curr_state = NORMAL;
            _tempsys->_prev_state = NORMAL;
            return;
        }
    }

    if(_tempsys->_curr_state == ERROR) {
        //_nf_trigger_error(_tempsys,);
        return;
    }

    _nf_max31855_result_t temp0_result;
    _nf_max31855_result_t temp1_result;

    if(_tempsys->_tempmode == USE_TEMP0 || _tempsys->_tempmode == USE_BOTH) {
        nf_max31855_read(_tempsys->_temp, NF_TEMP0_CS, &temp0_result);
        _nf_tempsys_update_temps(_tempsys, &temp0_result);

        if(temp0_result.faults > 0) {
            // We where using this sensor fine before this VERRY WEIRD and bad
            _tempsys->_curr_state = ERROR;
            _nf_trigger_error(_tempsys, TEMPSYS_FAULT_ERROR);
            return;
        }
    }
    
    if(_tempsys->_tempmode == USE_TEMP1 || _tempsys->_tempmode == USE_BOTH) {
        nf_max31855_read(_tempsys->_temp, NF_TEMP1_CS, &temp1_result);
        _nf_tempsys_update_temps(_tempsys, &temp1_result);

        if(temp1_result.faults > 0) {
            // We where using this sensor fine before this VERRY WEIRD and bad
            _tempsys->_curr_state = ERROR;
            _nf_trigger_error(_tempsys, TEMPSYS_FAULT_ERROR);
            return;
        }
    }

    _nf_swap_indexes(_tempsys);
    _nf_send_temp_update(_tempsys);

    // SANITY CHECK (normal just for now)
    if(_tempsys->_tempmode == USE_TEMP0) {
        _nf_temps_t* temps = (_nf_temps_t*) &(_tempsys->_results[0][_tempsys->read_index[0]]);
        if(_tempsys->_prev_state == NORMAL && _tempsys->_curr_state == CALIBRATION) {
            // Changed to calibration.
            _tempsys->integral = 0.f;
            _tempsys->prev_error = 0.f;
            _tempsys->pid_output = 0.f;

            _tempsys->pid_data[0] = _tempsys->_memory->current_buffer->pid_data[0][0];
            _tempsys->pid_data[1] = _tempsys->_memory->current_buffer->pid_data[0][1];
            _tempsys->pid_data[2] = _tempsys->_memory->current_buffer->pid_data[0][2];
            _tempsys->_prev_state = CALIBRATION;
            _tempsys->_pid_timeout = make_timeout_time_ms(450);
            return;
        }

        if (_tempsys->_curr_state == NORMAL) {
            if (temps->change_thermocouple > NORMAL_ALLOWED_CHANGERATE) {
                // SOMETHING BAD!
                _tempsys->_curr_state = ERROR;
                printf("%d", temps->change_thermocouple);
                _nf_trigger_error(_tempsys, TEMPSYS_TEMPRATURE_ERROR);
            }
           return;
        }

        if (_tempsys->_curr_state == CALIBRATION) {
            /*
            TODO (2023-12-18):
                Still to do get the PID values from the memory
                Also you still need to modify the menu screen so the calibration
                shows a chart so you can track the changes. 

                Also you still need to turn off the SSR if an error occours.
                secondly you might want to clean up the code a bit bit that's quite obvious atm.

                Other stuff? don't know the pid works kind off so that's nice :)
            */

           // Changed this to 70.f for a bit.
            if(get_absolute_time()._private_us_since_boot < _tempsys->_pid_timeout._private_us_since_boot) 
            {
                return;
            }

            _nf_pid_controller(_tempsys, 100.f);
            if(_tempsys->pid_output > 72.f + 1.5f) {
                gpio_put(NF_SSR0_PIN, 1);
                _tempsys->heater_state = 1;
            } else if (_tempsys->pid_output < 72.f - 1.5f) {
                gpio_put(NF_SSR0_PIN, 0);
                _tempsys->heater_state = 0;
            }

            // was 450
            _tempsys->_pid_timeout = make_timeout_time_ms(225);
            return;
        }
    }
}

void nf_tempsys_set_menu_queue(_nf_tempsys_t* _tempsys, queue_t* _menu_msg_queue_ptr)
{
    _tempsys->_menu_msq_queue_ptr = _menu_msg_queue_ptr;
}

void nf_tempsys_set_state(_nf_tempsys_t* _tempsys, uint new_state)
{
    _nf_tempsys_state_t state = _tempsys->_curr_state;
    if (state == ERROR || state == PRE_INIT || state == POST_INIT) {
        return;
    }

    if(new_state == 1) {
        state = NORMAL;
    }

    if(new_state == 2) {
        state = CALIBRATION;
    }

    _tempsys->_prev_state = _tempsys->_curr_state;
    _tempsys->_curr_state = state;
}

void _nf_pid_controller(_nf_tempsys_t* _tempsys, float setpoint)
{
    _nf_temps_t* temps = (_nf_temps_t*) &(_tempsys->_results[0][_tempsys->read_index[0]]);
    float error = setpoint - temps->thermocouple;

    float P = _tempsys->pid_data[0] * error;

    _tempsys->integral += _tempsys->pid_data[1] * error;
    if (_tempsys->integral > 100.0f) {
        _tempsys->integral = 100.0f;
    } else if (_tempsys->integral < 0.0f) {
        _tempsys->integral = 0.0f;
    }

    float D = _tempsys->pid_data[2] * (error - _tempsys->prev_error);
    _tempsys->pid_output = P + _tempsys->integral + D;

    _tempsys->prev_error = error;
}

void _nf_tempsys_update_temps(_nf_tempsys_t* _tempsys, _nf_max31855_result_t* results)
{
    // Pins are in order so this is a neat trick.
    uint index = (results->sensor - NF_TEMP0_CS);
    if(results->faults == 0) {
        _nf_temps_t* temps = (_nf_temps_t*) &(_tempsys->_results[index][_tempsys->write_index[index]]);
        temps->internal = results->internal;
        temps->thermocouple = results->thermocouple;
        temps->change_internal = 0.0;
        temps->change_thermocouple = 0.0;
        temps->bad_reading_count = 0;

        if(_tempsys->_curr_state != PRE_INIT) {
            _nf_temps_t* prev_temps = (_nf_temps_t*) &(_tempsys->_results[index][_tempsys->read_index[index]]);
            temps->change_internal = (temps->internal - prev_temps->internal);
            temps->change_thermocouple = (temps->thermocouple - prev_temps->thermocouple);

            if(abs(temps->change_internal) > 100 || abs(temps->change_thermocouple) > 100) {
                temps->bad_reading_count = prev_temps->bad_reading_count + 1;
            }
        }
    }
}

void _nf_swap_indexes(_nf_tempsys_t* _tempsys)
{
    uint tmp_windex_0 = _tempsys->write_index[0];
    uint tmp_windex_1 = _tempsys->write_index[1];
    _tempsys->write_index[0] = _tempsys->read_index[0];
    _tempsys->write_index[1] = _tempsys->read_index[1];
    
    _tempsys->read_index[0] = tmp_windex_0;
    _tempsys->read_index[1] = tmp_windex_1;
}

void _nf_send_temp_update(_nf_tempsys_t* _tempsys)
{
    if(_tempsys->_menu_msq_queue_ptr != NULL)
    {
        // TODO (DrNotThatEvil, 2024-02-05, 21:48): Extend this for mutiple sensors
        _nf_thread_msg temp_update_msg = { 
            .msg_type = TEMPSYS_MSG_TEMP_UPDATE_TYPE,
            .value_ptr = (void*) &(_tempsys->_results[0][_tempsys->read_index[0]])
        };
        queue_add_blocking(_tempsys->_menu_msq_queue_ptr, &temp_update_msg);
    }
}

void _nf_trigger_error(_nf_tempsys_t* _tempsys, uint error_flag)
{
    if(_tempsys->_menu_msq_queue_ptr != NULL)
    {
        _nf_thread_msg err_msg = { 
            .msg_type = TEMPSYS_MSG_ERROR_TYPE,
            .simple_msg_value = error_flag
        };

        queue_add_blocking(_tempsys->_menu_msq_queue_ptr, &err_msg);
    }
    //multicore_fifo_push_blocking(error_flag);

    // TODO (DrNotThatEvil, 2024-02-02, 14:05): Fix this for a better mechanisms
}