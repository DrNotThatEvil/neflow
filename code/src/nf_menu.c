#include "nf_menu.h"

void nf_menu_init(
    _nf_menu_t* _menu_state,
    ssd1306_t* _disp_ptr,
    tonegenerator_t* _tonegen,
    _nf_profile_state_t* _profile_state,
    _nf_memory_state_t* _memory,
    _nf_tempsys_t* _tempsys
) {
    _menu_state->btn_update_timeout = make_timeout_time_ms(NF_MENU_BTN_UPDATE_TIMEOUT_MS);

    for(int i=0; i < 3; i++) {
        _menu_state->_buttons[i].pressed = false;
        _menu_state->_buttons[i].held = false;
        _menu_state->_buttons[i].held_count = 0;
        _menu_state->_buttons[i].released = false;
    }

    _menu_state->_disp_ptr = _disp_ptr;
    _menu_state->_tonegen = _tonegen;
    _menu_state->_tempsys = _tempsys;

    _menu_state->_state = MENU_STATE_INIT;
    _menu_state->_error_state = {0};
    _menu_state->refresh_ms = 5;
    _menu_state->menu_screens = NULL; 
    _menu_state->cur_temp = 0.0;

    nf_main_menu_init(_menu_state);
    _menu_state->current_screen = &(_menu_state->menu_screens);

    nf_test_menu_init(_menu_state, _memory);

    nf_config_edit_menu_init(_menu_state, _memory);
    nf_profiles_menu_init(_menu_state, _profile_state);
    nf_profile_edit_menu_init(_menu_state, _profile_state);
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
    bool error_triggered = false;
    uint error_type = 0;
    uint error_animation = 0;
    uint32_t temp_started = multicore_fifo_pop_blocking();
    if(temp_started != TEMP_CORE_STARTED_FLAG) {
        error_triggered = true;
    }

    ssd1306_clear(_menu_state->_disp_ptr);
    for(;;)
    {
        if(error_triggered) {
            ssd1306_clear(_menu_state->_disp_ptr);
            if(error_animation == 0) {
                char str[20];
                sprintf(str, "ERROR: %d!!", error_type);
                ssd1306_draw_string(_menu_state->_disp_ptr, 5, 10, 2, str);
                ssd1306_draw_string(_menu_state->_disp_ptr, 5, 35, 1, "Reset oven!");
            }
            ssd1306_show(_menu_state->_disp_ptr);
            sleep_ms(1000);
            error_animation = (error_animation + 1) % 2;
            continue;
        } else {

            if (multicore_fifo_rvalid()) {
                uint32_t message = multicore_fifo_pop_blocking();
                
                if(message >= TEMP_ERROR_FLAG) {
                    error_triggered = true;
                    error_type = message;
                    continue;
                }
            }

            //_nf_temps_t* temp0_temps = (_nf_temps_t*) &(_menu_state->_tempsys->_results[0][_state->_tempsys->read_index[0]]);
            //nf_menu_update_cur_temp(_menu_state->_menu, temp0_temps->thermocouple);
            menu_update_btns();

            ssd1306_clear(_menu_state->_disp_ptr);
            nf_menu_render(_menu_state);
            ssd1306_show(_menu_state->_disp_ptr);

            sleep_ms(_menu_state->refresh_ms);
        }
    }
}

bool menu_can_update(_nf_menu_t* _menu_state)
{
    /*
     * TODO (DrNotThatEvil, 2024-01-09, 22:22):
     * This needs to be updated so the loop will error if temp_started does this on first run
     * but after that the loop should be fine to run, also if this fails you can move error logic in the
     * resulting if i think.
     * 
     * After this the temp update can be moved to the ui core in main.c
     * and the you can furture refactor the nf_config.h/nf_config.c stuff so it's less spagett
     * i'm liking how it's going so far tho.. so that's nice i guess.
    */ 

    bool error_triggered = false;
    uint error_type = 0;
    uint error_animation = 0;
    uint32_t temp_started = multicore_fifo_pop_blocking();
    if(temp_started != TEMP_CORE_STARTED_FLAG) {
        error_triggered = true;
    }
}

void menu_update_buttons(_nf_menu_t* _menu_state)
{
    if (get_absolute_time() < _menu_state->btn_update_timeout) {
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

uint nf_menu_get_menu_state(_nf_menu_t* _menu)
{
    if (_menu->_state == MENU_STATE_NORMAL) {
        return 0;
    }

    if (_menu->_state == MENU_STATE_CALIBRATION) {
        return 1;
    }
}

void nf_menu_btn_handler(_nf_menu_t* _menu, uint btn, bool repeat)
{
    //_menu->menu_options[_menu->current_menu_option].render_fn((void*)_menu, false);
    (*_menu->current_screen)->fnptrs.on_btn(_menu, btn, repeat, (*_menu->current_screen)->extra_data);
}

void nf_menu_render(_nf_menu_t* _menu)
{
    //_menu->menu_options[_menu->current_menu_option].render_fn((void*)_menu, false);
    (*_menu->current_screen)->fnptrs.on_render(_menu, _menu->_disp_ptr, (*_menu->current_screen)->extra_data);
}