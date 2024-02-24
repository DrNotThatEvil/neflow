#include "nf_menu.h"

void nf_menu_init(
    _nf_menu_t* _menu_state,
    tonegenerator_t* _tonegen,
    _nf_profile_state_t* _profile_state,
    _nf_memory_state_t* _memory,
    _nf_tempsys_t* _tempsys
) {

    _menu_state->btn_update_timeout = make_timeout_time_ms(NF_MENU_BTN_UPDATE_TIMEOUT_MS);
    _menu_state->scr_update_timeout = make_timeout_time_ms(_menu_state->refresh_ms);
    queue_init(&(_menu_state->tempsys_msg_q), sizeof(_nf_thread_msg), 4);

    _menu_state->refresh_ms = 5;
    for(int i=0; i < 3; i++) {
        _menu_state->_buttons[i].pressed = false;
        _menu_state->_buttons[i].held = false;
        _menu_state->_buttons[i].held_count = 0;
        _menu_state->_buttons[i].released = false;
    }

    _menu_state->_disp_ptr = (ssd1306_t*) malloc(sizeof(ssd1306_t));
    _menu_state->_disp_ptr->external_vcc = false;
    ssd1306_init(_menu_state->_disp_ptr, 128, 64, 0x3C, i2c1);

    _menu_state->_tonegen = _tonegen;
    _menu_state->_tempsys = _tempsys;

    _menu_state->_state = MENU_STATE_INIT;
    _menu_state->_error_state.error_type = 0;
    _menu_state->_error_state.error_animation = 0;

    _menu_state->menu_screens = NULL; 
    _menu_state->cur_temp = 0.0;
    _menu_state->heater_state = 0;

    nf_tempsys_set_menu_queue(_tempsys, &(_menu_state->tempsys_msg_q));
    nf_main_menu_init(_menu_state);
    _menu_state->current_screen = &(_menu_state->menu_screens);

    nf_test_menu_init(_menu_state, _memory);

    nf_config_edit_menu_init(_menu_state, _memory);
    nf_profiles_menu_init(_menu_state, _profile_state);
    nf_profile_edit_menu_init(_menu_state, _profile_state);
    nf_calibration_screen_init(_menu_state);
}

void menu_gpio_callback(_nf_menu_t* _menu, uint gpio, uint32_t events)
{
    if(events & GPIO_IRQ_EDGE_FALL) {
        if(debounce(gpio)) {
            return;
        }

        if (gpio == NF_MENU_BACK_BTN && !_menu->_buttons[NF_BACK_BTN_INDEX].pressed) {
            _menu->_buttons[NF_BACK_BTN_INDEX].pressed = true;
            nf_menu_btn_handler(_menu, 0, false);

            return;
            //_state->_menu->menu_options[_state->_menu->current_menu_option].back_fn((void*) _state->_menu, false);
        }

        if (gpio == NF_MENU_NEXT_BTN && !_menu->_buttons[NF_NEXT_BTN_INDEX].pressed) {
            _menu->_buttons[NF_NEXT_BTN_INDEX].pressed = true;
            nf_menu_btn_handler(_menu, 1, false);

            return;
            //_state->_menu->menu_options[_state->_menu->current_menu_option].next_fn((void*) _state->_menu, false);
        }

        if (gpio == NF_MENU_SELECT_BTN && !_menu->_buttons[NF_SELECT_BTN_INDEX].pressed) {
            _menu->_buttons[NF_SELECT_BTN_INDEX].pressed = true;
            nf_menu_btn_handler(_menu, 2, false);

            return;
            //_state->_menu->menu_options[_state->_menu->current_menu_option].select_fn((void*) _state->_menu, false);
        }
    }

    if(events & GPIO_IRQ_EDGE_RISE) {
        if (gpio == NF_MENU_BACK_BTN) {
            _menu->_buttons[NF_BACK_BTN_INDEX].released = true;
            //_state->_buttons[NF_BACK_BTN_INDEX].held = false;
            //_state->_buttons[NF_BACK_BTN_INDEX].pressed = false;
        }

        if (gpio == NF_MENU_NEXT_BTN) {
            _menu->_buttons[NF_NEXT_BTN_INDEX].released = true;
            //_state->_buttons[NF_NEXT_BTN_INDEX].held = false;
            //_state->_buttons[NF_NEXT_BTN_INDEX].pressed = false;
        }

        if (gpio == NF_MENU_SELECT_BTN) {
            _menu->_buttons[NF_SELECT_BTN_INDEX].released = true;
            //_state->_buttons[NF_SELECT_BTN_INDEX].held = false;
            //_state->_buttons[NF_SELECT_BTN_INDEX].pressed = false;
        }
    }
}

void menu_update(_nf_menu_t* _menu_state)
{
    if (get_absolute_time()._private_us_since_boot < _menu_state->scr_update_timeout._private_us_since_boot) {
        return;
    }

    bool error_triggered = menu_can_update(_menu_state);

    if(_menu_state->_state == MENU_STATE_ERROR || !error_triggered) {
        ssd1306_clear(_menu_state->_disp_ptr);
        if(_menu_state->_error_state.error_animation == 0) {
            char str[20];
            sprintf(str, "ERROR: %d!!", _menu_state->_error_state.error_type);
            ssd1306_draw_string(_menu_state->_disp_ptr, 5, 10, 2, str);
            ssd1306_draw_string(_menu_state->_disp_ptr, 5, 35, 1, "Reset oven!");
        }
        ssd1306_show(_menu_state->_disp_ptr);
        _menu_state->scr_update_timeout = make_timeout_time_ms(1000);
        _menu_state->_error_state.error_animation = (_menu_state->_error_state.error_animation + 1) % 2;
    } else {
        //_nf_temps_t* temp0_temps = (_nf_temps_t*) &(_menu_state->_tempsys->_results[0][_state->_tempsys->read_index[0]]);
        //nf_menu_update_cur_temp(_menu_state->_menu, temp0_temps->thermocouple);
        menu_handle_thread_messages(_menu_state);
        menu_update_buttons(_menu_state);

        nf_menu_render(_menu_state);
        ssd1306_show(_menu_state->_disp_ptr);
        _menu_state->scr_update_timeout = make_timeout_time_ms(_menu_state->refresh_ms);
    }
}

bool menu_can_update(_nf_menu_t* _menu_state)
{
    if(_menu_state->_state == MENU_STATE_INIT)
    {
        /*
        uint32_t temp_started = multicore_fifo_pop_blocking();
        if(temp_started != TEMP_CORE_STARTED_FLAG)
        {
            _menu_state->_state = MENU_STATE_ERROR;
            return false;
        }

        if(temp_started == TEMP_CORE_STARTED_FLAG)
        {
            _menu_state->_state = MENU_STATE_NORMAL;
            return true;
        }
        */

        _menu_state->_state = MENU_STATE_NORMAL;
        return true;
    }

    if(_menu_state->_state == MENU_STATE_ERROR)
    {
        return false;
    } 

    if(_menu_state->_state == MENU_STATE_NORMAL || _menu_state->_state == MENU_STATE_CALIBRATION)
    {
        if (!queue_is_empty(&_menu_state->tempsys_msg_q))
        {
            _nf_thread_msg msg; 
            queue_peek_blocking(&_menu_state->tempsys_msg_q, &msg);
            if((msg.msg_type & 0x0F) == 0)
            {
                // msg not for nf_menu;
                return true;
            }  


            if(msg.msg_type == TEMPSYS_ERROR_MSG_TYPE)
            {
                _menu_state->_state = MENU_STATE_ERROR;
                _menu_state->_error_state.error_type = msg.simple_msg_value;

                queue_remove_blocking(&_menu_state->tempsys_msg_q, &msg);
                return false;
            }
        }

        return true;
    }

    return false;
}

void menu_handle_thread_messages(_nf_menu_t* _menu_state)
{
    if (!queue_is_empty(&_menu_state->tempsys_msg_q))
    {
        _nf_thread_msg msg; 
        queue_peek_blocking(&_menu_state->tempsys_msg_q, &msg);
        if ((msg.msg_type & 0x0F) == 0) {
            // msg not ment for tempsys.
            return;
        }

        if(msg.msg_type == TEMPSYS_TEMP_UPDATE_MSG_TYPE)
        {
            _nf_temps_t* _temps = (_nf_temps_t*) msg.value_ptr;
            _menu_state->cur_temp = _temps->thermocouple;
        }

        // Always remove since msg was for us.
        queue_remove_blocking(&_menu_state->tempsys_msg_q, &msg);
    }
}

void menu_update_buttons(_nf_menu_t* _menu_state)
{
    if (get_absolute_time()._private_us_since_boot < _menu_state->btn_update_timeout._private_us_since_boot) {
        return;
    }
    
    _menu_state->btn_update_timeout = make_timeout_time_ms(NF_MENU_BTN_UPDATE_TIMEOUT_MS);

    for(int i=0; i < 3; i++) {
        if(_menu_state->_buttons[i].pressed && !_menu_state->_buttons[i].held && !_menu_state->_buttons[i].released) {
            _menu_state->_buttons[i].held = true;
        }

        if(_menu_state->_buttons[i].pressed && _menu_state->_buttons[i].held && !_menu_state->_buttons[i].released) {
            // Button being held repeat event here.
            // only back and next should have repeat events
            if(_menu_state->_buttons[i].held_count > NF_MENU_BTN_REPEAT_UNTIL_HELD) {
                if(i == NF_BACK_BTN_INDEX) {
                    nf_menu_btn_handler(_menu_state, 0, true);
                    //_state->_menu->menu_options[_state->_menu->current_menu_option].back_fn((void*) _state->_menu, true);
                }

                if(i == NF_NEXT_BTN_INDEX) {
                    nf_menu_btn_handler(_menu_state, 1, true);
                    //_state->_menu->menu_options[_state->_menu->current_menu_option].next_fn((void*) _state->_menu, true);
                }
            } else {
                _menu_state->_buttons[i].held_count = (_menu_state->_buttons[i].held_count + 1);
            }
            continue;
        }

        
        if(_menu_state->_buttons[i].released && _menu_state->_buttons[i].pressed) {
            // Fire released event here.. if i ever had one... (yes i'm picuteing the farily odd parents meme atm...)
            _menu_state->_buttons[i].released = false;
            _menu_state->_buttons[i].held = false;
            _menu_state->_buttons[i].held_count = 0;
            _menu_state->_buttons[i].pressed = false;
        }
    }
}

void nf_menu_update_cur_temp(_nf_menu_t* _menu, double temp)
{
    _menu->cur_temp = temp;
}

void nf_menu_update_heater_state(_nf_menu_t* _menu, uint state)
{
    _menu->heater_state = state;
}

uint nf_menu_get_menu_state(_nf_menu_t* _menu)
{
    if (_menu->_state == MENU_STATE_ERROR)
    {
        return -1;
    }

    if (_menu->_state == MENU_STATE_INIT)
    {
        return 0; 
    }

    if (_menu->_state == MENU_STATE_NORMAL) {
        return 1;
    }

    if (_menu->_state == MENU_STATE_CALIBRATION) {
        return 2;
    }

    if (_menu->_state == MENU_STATE_REFLOW) {
        return 3;
    }

    return -2;
}

void nf_menu_btn_handler(_nf_menu_t* _menu, uint btn, bool repeat)
{
    //_menu->menu_options[_menu->current_menu_option].render_fn((void*)_menu, false);
    (*_menu->current_screen)->fnptrs.on_btn(_menu, btn, repeat, (*_menu->current_screen)->extra_data);
}

void nf_menu_render(_nf_menu_t* _menu)
{
    //_menu->menu_options[_menu->current_menu_option].render_fn((void*)_menu, false);
    if((*_menu->current_screen)->autoclear) {
        ssd1306_clear(_menu->_disp_ptr);
    }

    (*_menu->current_screen)->fnptrs.on_render(_menu, _menu->_disp_ptr, (*_menu->current_screen)->extra_data);
}