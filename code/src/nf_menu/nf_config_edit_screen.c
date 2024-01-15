#include "nf_menu/nf_config_edit_screen.h"

void nf_config_edit_render(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data)
{
    _nf_config_edit_screen_state_t* config_edit_state = ((_nf_config_edit_screen_state_t*) extra_data);
    uint selected = config_edit_state->selected_value;
    uint set = config_edit_state->selected_set;
    uint change = config_edit_state->change;


    const char* config_sets[4] = { "Back", "Constant", "Reflow", "Save"};
    const char* config_options[4] = {"Back", "Kp", "Ki", "Kd" };
    const float change_values[3] = { 1.0f, 0.1f, 0.01f };

    if(set == 0) {
        for(uint i=0; i < 4; i++) {
            uint y = (i*10);
            draw_edit_item(disp_ptr, y, (selected == i), config_sets[i]);
        }

        return;
    }

    if(config_edit_state->editing == 0) {
        for(uint i=0; i < 4; i++) {
            uint y = (i*10);
            draw_edit_item(disp_ptr, y, (selected == i), config_options[i]);
        }

        return;
    }

    draw_edit_item(disp_ptr, 0, (config_edit_state->editing == 1 && !config_edit_state->adjust), "Back");
    draw_edit_item(disp_ptr, 11, (config_edit_state->editing == 2 && !config_edit_state->adjust), config_options[selected]);

    float value = config_edit_state->_memory_state->current_buffer->pid_data[set - 1][selected - 1];
    char str[20];
    sprintf(str, "%.2f", value);
    draw_edit_value(disp_ptr, 22, (config_edit_state->editing == 2 && config_edit_state->adjust), str);
    
    draw_edit_item(disp_ptr, 34, (config_edit_state->editing == 3 && !config_edit_state->adjust), "Change:");
    sprintf(str, "%.2f", change_values[change]);
    draw_edit_value(disp_ptr, 45, (config_edit_state->editing == 3 && config_edit_state->adjust), str);
}

void nf_config_edit_btn_handler(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data)
{
    _nf_config_edit_screen_state_t* config_edit_state = ((_nf_config_edit_screen_state_t*) extra_data);
    const float change_values[3] = { 1.0f, 0.1f, 0.01f };
    
    uint tone_duration = 100;
    if(repeat) {
        tone_duration = 50;
    }

    if(btn == 0) {
        if (config_edit_state->editing == 0) {
            if(config_edit_state->selected_value > 0) {
                config_edit_state->selected_value = (config_edit_state->selected_value - 1) % 4;
            }
            return;
        }

        if(!config_edit_state->adjust) {
            if(config_edit_state->editing > 1) {
                config_edit_state->editing = (config_edit_state->editing - 1) % 3;
            }
        }

        if(config_edit_state->adjust) {
            if(config_edit_state->editing == 2) {
                uint set = config_edit_state->selected_set - 1;
                uint selected = config_edit_state->selected_value - 1;
                float* value = &(config_edit_state->_memory_state->current_buffer->pid_data[set][selected]);

                if (*value > 0.01f) {
                    if((*value - change_values[config_edit_state->change]) > 0.01f && (*value - change_values[config_edit_state->change]) < 100.0f){
                        *value = (*value - change_values[config_edit_state->change]);
                    } else {
                        *value = 0.01f;
                    }
                } 
            }

            if(config_edit_state->editing == 3) {
                if(config_edit_state->change > 0) {
                    config_edit_state->change = (config_edit_state->change - 1) % 3;
                }
            }
        }
        /*
        if(config_edit_state->adjust) {
            uint selected = config_edit_state->selected_value - 1;
            uint edit = config_edit_state->editing - 2;
            uint* value = &(profile_edit_state->current_profile->targets[selected][edit]);

            if(edit == 0 || selected == 0) {
                if (*value > 1) {
                    *value = (*value - 1);
                    tone(menu_state->_tonegen, NOTE_C3, tone_duration);
                }
            } else {
                uint* value_prev = &(profile_edit_state->current_profile->targets[selected-1][1]);

                if (*value > *value_prev) {
                    *value = *value - 1;
                    tone(menu_state->_tonegen, NOTE_C3, tone_duration);
                }
            }
        }
        */
    }


    if(btn == 1) {
        if (config_edit_state->editing == 0) {
            config_edit_state->selected_value = (config_edit_state->selected_value + 1) % 4;
            return;
        }

        if(!config_edit_state->adjust) {
            config_edit_state->editing = (config_edit_state->editing % 3) + 1;
        } 
        
        if(config_edit_state->adjust) {
            if(config_edit_state->editing == 2) {
                uint set = config_edit_state->selected_set - 1;
                uint selected = config_edit_state->selected_value - 1;
                float* value = &(config_edit_state->_memory_state->current_buffer->pid_data[set][selected]);

                if (*value < 100.0f) {
                    if((*value + change_values[config_edit_state->change]) > 0.0f && (*value + change_values[config_edit_state->change]) < 100.0f){
                        *value = (*value + change_values[config_edit_state->change]);
                    } else {
                        *value = 100.0f;
                    }
                } 
            }

            if(config_edit_state->editing == 3) {
                config_edit_state->change = (config_edit_state->change + 1) % 3;
            }
        }

        /*
        if(profile_edit_state->adjust) {
            uint selected = profile_edit_state->selected_value - 1;
            uint edit = profile_edit_state->editing - 2;
            uint* value = &(profile_edit_state->current_profile->targets[selected][edit]);

            if (*value < 229) {
                *value = (*value + 1);
                tone(menu_state->_tonegen, NOTE_A3, tone_duration);
            }
        }
        */
    }

    if(btn == 2 && !repeat) {
        if (config_edit_state->selected_set == 0 && config_edit_state->selected_value == 0 && config_edit_state->editing == 0) {
            tone(menu_state->_tonegen, NOTE_C3, tone_duration);
            nf_menu_change_screen(menu_state, MAIN_MENU_SCREEN_ID);
            return;
        }
        
        if (config_edit_state->selected_set > 0 && config_edit_state->selected_value == 0 && config_edit_state->editing == 0) {
            tone(menu_state->_tonegen, NOTE_C3, tone_duration);
            config_edit_state->selected_set = 0;
            return;
        }

        if (config_edit_state->editing == 0) {
            if(config_edit_state->selected_set == 0) {
                if(config_edit_state->selected_value > 0 &&  config_edit_state->selected_value < 3) {
                    config_edit_state->selected_set = config_edit_state->selected_value;
                    config_edit_state->selected_value = 0;

                    tone(menu_state->_tonegen, NOTE_A4, tone_duration);
                    return;
                }

                if(config_edit_state->selected_value == 3) {
                    //tone(menu_state->_tonegen, NOTE_A4, tone_duration);

                    nf_memory_save(config_edit_state->_memory_state);
                    //nf_memory_save_with_callback(config_edit_state->_memory_state, nf_config_edit_cb, (void*)menu_state);
                }
            }
            
            if(config_edit_state->selected_set > 0) {
                if(config_edit_state->selected_value > 0) {
                    config_edit_state->editing = 1;
            
                    tone(menu_state->_tonegen, NOTE_A4, tone_duration);
                }
            }
        } else {
            if (config_edit_state->editing > 1) {
                config_edit_state->adjust = !config_edit_state->adjust;

                if (config_edit_state->adjust) {
                    tone(menu_state->_tonegen, NOTE_A4, tone_duration);
                } else {
                    tone(menu_state->_tonegen, NOTE_C4, tone_duration);
                }
            } else {
                config_edit_state->editing = 0;
                tone(menu_state->_tonegen, NOTE_C4, tone_duration);
            }
        }
    }
}

void nf_config_edit_cb(void* _nf_menu_vptr)
{
    _nf_menu_t* _menu_state = (_nf_menu_t*) _nf_menu_vptr;
    tone(_menu_state->_tonegen, NOTE_A4, 100);
}

void nf_config_edit_menu_init(_nf_menu_t* _menu_state, _nf_memory_state_t* _memory_state_ptr)
{
    _nf_menu_screen_t** _menu_screens_ptr = &_menu_state->menu_screens;
    _nf_menu_screen_t* config_edit_screen = (_nf_menu_screen_t*) malloc(sizeof(_nf_menu_screen_t));

    config_edit_screen->id = CONFIG_SCREEN_ID;
    config_edit_screen->extra_data = malloc(sizeof(_nf_config_edit_screen_state_t));

    _nf_config_edit_screen_state_t* config_edit_state = (_nf_config_edit_screen_state_t*) config_edit_screen->extra_data;
    config_edit_state->_memory_state = _memory_state_ptr;
    config_edit_state->selected_value = 0;
    config_edit_state->selected_set = 0;
    config_edit_state->editing = 0;
    config_edit_state->adjust = false;
    config_edit_state->change = 0;

    _nf_menu_screen_fn_ptrs_t config_edit_screen_fns = {
        .on_render = nf_config_edit_render,
        .on_btn = nf_config_edit_btn_handler,
        .on_active = NULL
    };
    
    config_edit_screen->fnptrs = config_edit_screen_fns;
    config_edit_screen->next = NULL;


    nf_menu_add_screen(_menu_screens_ptr, config_edit_screen);
}

