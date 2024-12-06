#include "nf_tempsys.h"

void nf_tempsys_init(_nf_tempsys_t *_tempsys, _nf_memory_state_t *_memory)
{
    _tempsys->_memory = _memory;
    _tempsys->_temp = malloc(sizeof(_nf_max31855_t));

    _tempsys->_menu_msq_queue_ptr = ((void *)0);
    _tempsys->_profile = ((void *)0);
    _tempsys->_cur_stage_index = 0;
    _tempsys->_cur_stage_l_cnt = 0;

    nf_max31855_init(_tempsys->_temp);

    _tempsys->_curr_state = PRE_INIT;
    _tempsys->_prev_state = PRE_INIT;
    _tempsys->_tempmode = UNKNOWN;
    _tempsys->heater_state = 0;

    for (uint i = 0; i < 2; i++)
    {
        for (uint j = 0; j < 2; j++)
        {
            _nf_temps_t empty_temp = {.internal = 0.0,
                                      .thermocouple = 0.0,
                                      .change_internal = 0.0,
                                      .change_thermocouple = 0.0};
            _tempsys->_results[i][j] = empty_temp;
        }
    }

    _tempsys->bootup_temp = 0.0;
    _tempsys->hit_target = false;

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

void nf_tempsys_update(_nf_tempsys_t *_tempsys)
{
    // Initaization takes 2 loops, one to determine tempmode and read frist
    // temp. Second one does the same but also gets correct change from previous
    // reading. else first reading would be a verry high change rate.
    if (_tempsys->_curr_state == PRE_INIT || _tempsys->_curr_state == POST_INIT)
    {
        if (_tempsys->_menu_msq_queue_ptr == ((void *)0))
        {
            // Do not start if we don't have a queue yet.
            return;
        }

        // Determine temprature mode
        _nf_max31855_result_t temp0_result;
        nf_max31855_read(_tempsys->_temp, NF_TEMP0_CS, &temp0_result);
        _nf_tempsys_update_temps(_tempsys, &temp0_result);

        _nf_max31855_result_t temp1_result;
        nf_max31855_read(_tempsys->_temp, NF_TEMP1_CS, &temp1_result);
        _nf_tempsys_update_temps(_tempsys, &temp1_result);

        if (temp0_result.faults == 0 && temp1_result.faults == 0)
        {
            _tempsys->_tempmode = USE_BOTH;
        }
        else if (temp0_result.faults == 0 && temp1_result.faults > 0)
        {
            _tempsys->_tempmode = USE_TEMP0;
        }
        else if (temp0_result.faults > 0 && temp1_result.faults == 0)
        {
            _tempsys->_tempmode = USE_TEMP1;
        }
        else
        {
            _tempsys->_curr_state = ERROR;
            return;
        }

        _nf_swap_indexes(_tempsys);

        if (_tempsys->_curr_state == PRE_INIT)
        {
            _nf_tempsys_set_state(_tempsys, POST_INIT);
            //_tempsys->_curr_state = POST_INIT;
            //_tempsys->_prev_state = POST_INIT;
            return;
        }
        else
        {
            _nf_tempsys_set_state(_tempsys, NORMAL);

            // here, use the correct sensor.
            _tempsys->bootup_temp = 0.0;

            // defualt use SENSOR0
            uint8_t min_sensor = 0;
            uint8_t max_sensor = 1;
            if (_tempsys->_tempmode == USE_TEMP1)
            {
                min_sensor = 1;
                max_sensor = 2;
            }
            else if (_tempsys->_tempmode == USE_BOTH)
            {
                max_sensor = 2;
            }

            for (uint8_t i = min_sensor; i < max_sensor; i++)
            {
                _tempsys->bootup_temp =
                    ((_nf_temps_t *)&(
                         _tempsys->_results[i][_tempsys->read_index[i]]))
                        ->thermocouple;
            }

            // max sensor - min sensor makes sure that the divide works when
            // only using TEMP0 or TEMP1.
            _tempsys->bootup_temp =
                _tempsys->bootup_temp / (max_sensor - min_sensor);

            return;
        }
    }

    if (_tempsys->_curr_state == ERROR)
    {
        // We still wanna handle msgs, to prevent deadlock.

        // if we go into error state we push a message to the queue
        // but before we do so the menu pushes a message
        // if we would not remove that message (because of the error state)
        // the message would prevent the menu from seeing our error message.
        _nf_tempsys_handle_thread_messages(_tempsys);
        return;
    }

    _nf_max31855_result_t temp0_result;
    _nf_max31855_result_t temp1_result;

    if (_tempsys->_tempmode == USE_TEMP0 || _tempsys->_tempmode == USE_BOTH)
    {
        nf_max31855_read(_tempsys->_temp, NF_TEMP0_CS, &temp0_result);
        _nf_tempsys_update_temps(_tempsys, &temp0_result);

        if (temp0_result.faults > 0)
        {
            // We where using this sensor fine before this VERRY WEIRD and bad
            _tempsys->_curr_state = ERROR;
            _nf_trigger_error(_tempsys, TEMPSYS_FAULT_ERROR);
            return;
        }
    }

    if (_tempsys->_tempmode == USE_TEMP1 || _tempsys->_tempmode == USE_BOTH)
    {
        nf_max31855_read(_tempsys->_temp, NF_TEMP1_CS, &temp1_result);
        _nf_tempsys_update_temps(_tempsys, &temp1_result);

        if (temp1_result.faults > 0)
        {
            // We where using this sensor fine before this VERRY WEIRD and bad
            _tempsys->_curr_state = ERROR;
            _nf_trigger_error(_tempsys, TEMPSYS_FAULT_ERROR);
            return;
        }
    }

    _nf_tempsys_handle_thread_messages(_tempsys);
    _nf_swap_indexes(_tempsys);
    _nf_sanity_check(_tempsys);
    _nf_send_temp_update(_tempsys);

    _nf_temps_t *temps;
    if (_tempsys->_tempmode == USE_TEMP0)
    {
        temps =
            (_nf_temps_t *)&(_tempsys->_results[0][_tempsys->read_index[0]]);
        if (_tempsys->_prev_state == NORMAL &&
            _tempsys->_curr_state == CALIBRATION)
        {
            // Changed to calibration.
            _tempsys->integral = 0.f;
            _tempsys->prev_error = 0.f;
            _tempsys->pid_output = 0.f;

            _tempsys->pid_data[0] =
                _tempsys->_memory->current_buffer->pid_data[0][0];
            _tempsys->pid_data[1] =
                _tempsys->_memory->current_buffer->pid_data[0][1];
            _tempsys->pid_data[2] =
                _tempsys->_memory->current_buffer->pid_data[0][2];
            _tempsys->_prev_state = CALIBRATION;
            _tempsys->_pid_timeout = make_timeout_time_ms(225);
            return;
        }

        if (_tempsys->_prev_state == NORMAL && _tempsys->_curr_state == RUNNING)
        {
            // Changed to RUNNING.
            _tempsys->integral = 0.f;
            _tempsys->prev_error = 0.f;
            _tempsys->pid_output = 0.f;

            _tempsys->pid_data[0] =
                _tempsys->_memory->current_buffer->pid_data[1][0];
            _tempsys->pid_data[1] =
                _tempsys->_memory->current_buffer->pid_data[1][1];
            _tempsys->pid_data[2] =
                _tempsys->_memory->current_buffer->pid_data[1][2];
            _tempsys->_prev_state = RUNNING;
            _tempsys->_cur_stage_l_cnt = 0;
            _tempsys->hit_target = false;

            _tempsys->_pid_timeout = make_timeout_time_ms(200);
            return;
        }

        if (_tempsys->_curr_state == CALIBRATION)
        {
            // Changed this to 70.f for a bit.
            if (get_absolute_time() < _tempsys->_pid_timeout)
            {
                return;
            }

            _nf_pid_controller(_tempsys, 100.f);
            if (_tempsys->pid_output > 72.f + 1.5f)
            {
                gpio_put(NF_SSR0_PIN, 1);
                _tempsys->heater_state = 1;
            }
            else if (_tempsys->pid_output < 72.f - 1.5f)
            {
                gpio_put(NF_SSR0_PIN, 0);
                _tempsys->heater_state = 0;
            }

            // was 450
            _tempsys->_pid_timeout = make_timeout_time_ms(200);
            return;
        }

        if (_tempsys->_curr_state == RUNNING)
        {
            // Changed this to 70.f for a bit.
            if (get_absolute_time() < _tempsys->_pid_timeout)
            {
                return;
            }

            if (_tempsys->_cur_stage_index >= PROFILE_TARGETS_SIZE)
            {
                // implement finishing here... should actually already be done.
                gpio_put(NF_SSR0_PIN, 0);
                return;
            }

            float cur_total_target =
                ((float)_tempsys->_profile
                     ->targets[_tempsys->_cur_stage_index][0]);

            if (!_tempsys->hit_target &&
                temps->thermocouple >= cur_total_target)
            {
                _tempsys->hit_target = true;
            }

            _tempsys->_cur_stage_l_cnt++;

            float cur_time =
                ((float)((float)_tempsys->_cur_stage_l_cnt * 200.f) / 1000.0);
            uint16_t prev_time = 0;
            if (_tempsys->_cur_stage_index > 0)
            {
                prev_time = _tempsys->_profile
                                ->targets[(_tempsys->_cur_stage_index - 1)][1];
            }

            // if next_target > current_unhit => allow
            // if next_target > current_hit => allow
            // if next_target < current_unhit => NO

            // minus the previous stage
            uint16_t cur_target_time =
                _tempsys->_profile->targets[_tempsys->_cur_stage_index][1] -
                prev_time;
            if (cur_time > cur_target_time)
            {
                float next_target = cur_total_target;
                // 0 -> 1 = next
                // 1 -> 2 = next
                // 2 -> 3 = next
                // 3 -> 4 = current
                if ((_tempsys->_cur_stage_index + 1) < PROFILE_TARGETS_SIZE)
                {
                    next_target =
                        ((float)_tempsys->_profile
                             ->targets[(_tempsys->_cur_stage_index + 1)][0]);
                }

                if ((next_target >= cur_total_target) ||
                    (next_target < cur_total_target && _tempsys->hit_target))
                {
                    _tempsys->_cur_stage_l_cnt = 0;
                    _tempsys->_cur_stage_index++;
                    _tempsys->hit_target = false;

                    if (_tempsys->_cur_stage_index >= PROFILE_TARGETS_SIZE &&
                        _tempsys->_curr_state != FINISHED)
                    {
                        gpio_put(NF_SSR0_PIN, 0);

                        _tempsys->_curr_state = FINISHED;
                        _nf_send_finished(_tempsys);
                        return;
                    }
                }
            }

            float cur_target_temp = _nf_calculate_target_temp(_tempsys);

            _nf_pid_controller(_tempsys, cur_target_temp);
            if (_tempsys->pid_output > 72.f + 1.5f)
            {
                gpio_put(NF_SSR0_PIN, 1);
                _tempsys->heater_state = 1;
            }
            else if (_tempsys->pid_output < 72.f - 1.5f)
            {
                gpio_put(NF_SSR0_PIN, 0);
                _tempsys->heater_state = 0;
            }

            // was 450
            _tempsys->_pid_timeout = make_timeout_time_ms(200);
            return;
        }
    }
}

float _nf_calculate_target_temp(_nf_tempsys_t *_tempsys)
{
    _nf_temps_t *temps =
        (_nf_temps_t *)&(_tempsys->_results[0][_tempsys->read_index[0]]);
    float prev_target_temp = temps->thermocouple;
    uint16_t prev_target_time = 0;

    float cur_target_temp =
        ((float)_tempsys->_profile->targets[_tempsys->_cur_stage_index][0]);
    uint16_t cur_target_time =
        _tempsys->_profile->targets[_tempsys->_cur_stage_index][1];

    if (_tempsys->_cur_stage_index > 0)
    {
        prev_target_temp = ((float)_tempsys->_profile
                                ->targets[(_tempsys->_cur_stage_index - 1)][0]);
        prev_target_time =
            _tempsys->_profile->targets[(_tempsys->_cur_stage_index - 1)][1];
    }

    float total_change = cur_target_temp - prev_target_temp;
    float temp_per_s = total_change / (cur_target_time - prev_target_time);
    float temp_adj =
        prev_target_temp + ((temp_per_s / 0.2f) * _tempsys->_cur_stage_l_cnt);

    if ((total_change > 0.0f && temp_adj > cur_target_temp) ||
        (total_change < 0.0f && temp_adj < cur_target_temp))
    {
        temp_adj = cur_target_temp;
    }

    return temp_adj;
}

void nf_tempsys_set_menu_queue(_nf_tempsys_t *_tempsys,
                               queue_t *_menu_msg_queue_ptr)
{
    _tempsys->_menu_msq_queue_ptr = _menu_msg_queue_ptr;
}

void _nf_sanity_check(_nf_tempsys_t *_tempsys)
{
    _nf_temps_t *temps =
        (_nf_temps_t *)&(_tempsys->_results[0][_tempsys->read_index[0]]);
    if (_tempsys->_curr_state == RUNNING || _tempsys->_curr_state == FINISHED)
    {
        if (temps->change_thermocouple > RUNNING_ALLOWED_CHANGE_RATE)
        {
            _tempsys->_curr_state = ERROR;
            _nf_trigger_error(_tempsys, TEMPSYS_TEMPRATURE_CHANGERATE_ERROR);
            return;
        }

        if (temps->thermocouple > RUNNING_TO_HIGH_TEMP)
        {
            _tempsys->_curr_state = ERROR;
            _nf_trigger_error(_tempsys, TEMPSYS_TEMPRATURE_HIGH_ERROR);
            return;
        }

        if (temps->thermocouple < NORMAL_TO_LOW_TEMP)
        {
            _tempsys->_curr_state = ERROR;
            _nf_trigger_error(_tempsys, TEMPSYS_TEMPRATURE_TO_LOW_ERROR);
        }

        return;
    }

    if (_tempsys->_curr_state == CALIBRATION)
    {
        if (temps->change_thermocouple > CALIBRATION_ALLOWED_CHANGE_RATE)
        {
            _tempsys->_curr_state = ERROR;
            _nf_trigger_error(_tempsys, TEMPSYS_TEMPRATURE_CHANGERATE_ERROR);
            return;
        }

        if (temps->thermocouple > CALIBRATION_TO_HIGH_TEMP)
        {
            _tempsys->_curr_state = ERROR;
            _nf_trigger_error(_tempsys, TEMPSYS_TEMPRATURE_HIGH_ERROR);
            return;
        }

        if (temps->thermocouple < NORMAL_TO_LOW_TEMP)
        {
            _tempsys->_curr_state = ERROR;
            _nf_trigger_error(_tempsys, TEMPSYS_TEMPRATURE_TO_LOW_ERROR);
        }

        return;
    }

    // Default to NORMAL values if other states have not captured anything.
    if (temps->change_thermocouple > NORMAL_ALLOWED_CHANGE_RATE)
    {
        _tempsys->_curr_state = ERROR;
        _nf_trigger_error(_tempsys, TEMPSYS_TEMPRATURE_CHANGERATE_ERROR);
        return;
    }

    if (temps->thermocouple > NORMAL_TO_HIGH_TEMP)
    {
        // This check prevents this trigger if the boot up temp was higher then
        // the trigger temp but keeps it triggering if the temp keeps rising
        if (_tempsys->bootup_temp < NORMAL_TO_HIGH_TEMP ||
            temps->change_thermocouple > 1.0)
        {
            _tempsys->_curr_state = ERROR;
            _nf_trigger_error(_tempsys, TEMPSYS_TEMPRATURE_HIGH_ERROR);
        }
    }

    if (temps->thermocouple < NORMAL_TO_LOW_TEMP)
    {
        _tempsys->_curr_state = ERROR;
        _nf_trigger_error(_tempsys, TEMPSYS_TEMPRATURE_TO_LOW_ERROR);
    }
}

void _nf_tempsys_set_state(_nf_tempsys_t *_tempsys,
                           _nf_tempsys_state_t new_state)
{
    _nf_tempsys_state_t state = _tempsys->_curr_state;
    if (state == ERROR)
    {
        return;
    }

    if ((state == PRE_INIT || state == POST_INIT) &&
        (new_state == POST_INIT || new_state == NORMAL))
    {
        _tempsys->_prev_state = new_state;
        _tempsys->_curr_state = new_state;

        if (new_state == NORMAL)
        {
            _nf_send_temp_initialized(_tempsys);
        }

        return;
    }

    _tempsys->_prev_state = _tempsys->_curr_state;
    _tempsys->_curr_state = new_state;
}

void _nf_pid_controller(_nf_tempsys_t *_tempsys, float setpoint)
{
    _nf_temps_t *temps =
        (_nf_temps_t *)&(_tempsys->_results[0][_tempsys->read_index[0]]);
    float error = setpoint - temps->thermocouple;

    float P = _tempsys->pid_data[0] * error;

    _tempsys->integral += _tempsys->pid_data[1] * error;
    if (_tempsys->integral > 100.0f)
    {
        _tempsys->integral = 100.0f;
    }
    else if (_tempsys->integral < 0.0f)
    {
        _tempsys->integral = 0.0f;
    }

    float D = _tempsys->pid_data[2] * (error - _tempsys->prev_error);
    _tempsys->pid_output = P + _tempsys->integral + D;

    _tempsys->prev_error = error;
}

void _nf_tempsys_update_temps(_nf_tempsys_t *_tempsys,
                              _nf_max31855_result_t *results)
{
    // Pins are in order so this is a neat trick.
    uint index = (results->sensor - NF_TEMP0_CS);
    if (results->faults == 0)
    {
        _nf_temps_t *temps = (_nf_temps_t *)&(
            _tempsys->_results[index][_tempsys->write_index[index]]);
        temps->internal = results->internal;
        temps->thermocouple = results->thermocouple;
        temps->change_internal = 0.0;
        temps->change_thermocouple = 0.0;
        temps->bad_reading_count = 0;

        if (_tempsys->_curr_state != PRE_INIT)
        {
            _nf_temps_t *prev_temps = (_nf_temps_t *)&(
                _tempsys->_results[index][_tempsys->read_index[index]]);
            temps->change_internal = (temps->internal - prev_temps->internal);
            temps->change_thermocouple =
                (temps->thermocouple - prev_temps->thermocouple);

            if (abs(temps->change_internal) > 100 ||
                abs(temps->change_thermocouple) > 100)
            {
                temps->bad_reading_count = prev_temps->bad_reading_count + 1;
                return;
            }

            if (temps->change_thermocouple < 0.0 &&
                _tempsys->bootup_temp > NORMAL_TO_HIGH_TEMP)
            {
                // If temprature is falling and bootup temp was higher then
                // trigger, reset this if now lower.
                if (temps->thermocouple < (NORMAL_TO_HIGH_TEMP - 2.0))
                {
                    _tempsys->bootup_temp = temps->thermocouple;
                }
            }
        }
    }
}

void _nf_tempsys_handle_thread_messages(_nf_tempsys_t *_tempsys)
{
    if (_tempsys->_menu_msq_queue_ptr == ((void *)0))
    {
        return;
    }

    if (!queue_is_empty(_tempsys->_menu_msq_queue_ptr))
    {
        _nf_thread_msg msg;
        queue_peek_blocking(_tempsys->_menu_msq_queue_ptr, &msg);
        if ((msg.msg_type & 0xF0) == 0)
        {
            // msg not ment for tempsys.
            return;
        }

        _nf_tempsys_state_t state = _tempsys->_curr_state;
        if (state != ERROR)
        {
            if (state != RUNNING && msg.msg_type == MENU_SET_PROFILE_MSG_TYPE)
            {
                // Allow setting profile if not running (should not occour but
                // better to be safe.)
                _tempsys->_profile = msg.value_ptr;
                _tempsys->_cur_stage_index = 0;
            }

            if (msg.msg_type == MENU_STATE_CHANGE_MSG_TYPE)
            {
                if (msg.simple_msg_value == 2)
                {
                    state = NORMAL;
                }

                if (msg.simple_msg_value == 3)
                {
                    state = CALIBRATION;
                }

                if (msg.simple_msg_value == 4)
                {
                    state = RUNNING;
                }

                _nf_tempsys_set_state(_tempsys, state);
            }

            // Always remove since the msg was for us.
            queue_remove_blocking(_tempsys->_menu_msq_queue_ptr, &msg);
        }
    }
}

void _nf_swap_indexes(_nf_tempsys_t *_tempsys)
{
    uint tmp_windex_0 = _tempsys->write_index[0];
    uint tmp_windex_1 = _tempsys->write_index[1];
    _tempsys->write_index[0] = _tempsys->read_index[0];
    _tempsys->write_index[1] = _tempsys->read_index[1];

    _tempsys->read_index[0] = tmp_windex_0;
    _tempsys->read_index[1] = tmp_windex_1;
}

void _nf_send_temp_initialized(_nf_tempsys_t *_tempsys)
{
    if (_tempsys->_menu_msq_queue_ptr == ((void *)0))
    {
        return;
    }

    _nf_thread_msg initialized_msg = {.msg_type = TEMPSYS_INITIALIZED_MSG_TYPE,
                                      .simple_msg_value = 0};

    queue_add_blocking(_tempsys->_menu_msq_queue_ptr, &initialized_msg);
}

void _nf_send_temp_update(_nf_tempsys_t *_tempsys)
{
    if (_tempsys->_menu_msq_queue_ptr == ((void *)0))
    {
        return;
    }

    uint16_t flags = 0;
    if (_tempsys->_tempmode == USE_TEMP0)
    {
        SET_TEMPSYS_TEMP0_READABLE(flags);
        SET_TEMPSYS_TEMP0_READABLE_INDEX(flags, _tempsys->read_index[0]);
    }
    else if (_tempsys->_tempmode == USE_TEMP1)
    {
        SET_TEMPSYS_TEMP1_READABLE(flags);
        SET_TEMPSYS_TEMP1_READABLE_INDEX(flags, _tempsys->read_index[1]);
    }
    else if (_tempsys->_tempmode == USE_BOTH)
    {
        SET_TEMPSYS_TEMP0_READABLE(flags);
        SET_TEMPSYS_TEMP1_READABLE(flags);

        SET_TEMPSYS_TEMP0_READABLE_INDEX(flags, _tempsys->read_index[0]);
        SET_TEMPSYS_TEMP1_READABLE_INDEX(flags, _tempsys->read_index[1]);
    }

    _nf_thread_msg temp_update_msg = {
        .msg_type = TEMPSYS_TEMP_UPDATE_MSG_TYPE,
        .simple_msg_value = flags,
        .value_ptr = (void *)_tempsys->_results,
    };
    queue_add_blocking(_tempsys->_menu_msq_queue_ptr, &temp_update_msg);
}

void _nf_send_finished(_nf_tempsys_t *_tempsys)
{
    if (_tempsys->_menu_msq_queue_ptr == ((void *)0))
    {
        return;
    }

    _nf_thread_msg finished_msg = {.msg_type = TEMPSYS_FINISHED_MSG_TYPE};
    queue_add_blocking(_tempsys->_menu_msq_queue_ptr, &finished_msg);
}

void _nf_trigger_error(_nf_tempsys_t *_tempsys, uint error_flag)
{
    ///////////////////////////////////////////////////////////////////////
    // NOTE (DrNotThatEvil, 2024-12-05): Critical systems need to be turned of
    // on error. For now that's only the SSR0 but in the future might need to
    // extend this list below or make a macro that expands it for you.

    gpio_put(NF_SSR0_PIN, 0);

    if (_tempsys->_menu_msq_queue_ptr == ((void *)0))
    {
        return;
    }

    _nf_tempsys_state_t state = _tempsys->_curr_state;
    uint8_t add = 0;
    if (state == CALIBRATION)
    {
        add = 10;
    }

    if (state == RUNNING)
    {
        add = 20;
    }

    _nf_thread_msg err_msg = {.msg_type = TEMPSYS_ERROR_MSG_TYPE,
                              .simple_msg_value = (error_flag + add)};

    queue_add_blocking(_tempsys->_menu_msq_queue_ptr, &err_msg);
}
