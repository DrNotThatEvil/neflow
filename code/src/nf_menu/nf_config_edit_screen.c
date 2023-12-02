#include "nf_menu/nf_config_edit_screen.h"

void nf_config_edit_render(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data)
{
    _nf_config_edit_screen_state_t* config_edit_state = ((_nf_config_edit_screen_state_t*) extra_data);
    uint selected = config_edit_state->selected_value;

    if(config_edit_state->editing == 0) {
        draw_edit_item(disp_ptr, 0, (selected == 0), "Back");
        draw_edit_item(disp_ptr, 11, (selected == 1), "SOME");
        draw_edit_item(disp_ptr, 22, (selected == 2), "Somethingelse");
        draw_edit_item(disp_ptr, 33, (selected == 3), "Another");
        draw_edit_item(disp_ptr, 53, (selected == 4), "Save");

        return;
    }

    draw_edit_item(disp_ptr, 0, (config_edit_state->editing == 1 && !config_edit_state->adjust), "Back");
    draw_edit_item(disp_ptr, 11, (config_edit_state->editing == 2 && !config_edit_state->adjust), "Target temp:");

    char str[20];
    sprintf(str, "%d", 10);
    draw_edit_value(disp_ptr, 22, (config_edit_state->editing == 2 && config_edit_state->adjust), str);
    
    
}

void nf_config_edit_btn_handler(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data)
{
    _nf_config_edit_screen_state_t* config_edit_state = ((_nf_config_edit_screen_state_t*) extra_data);
    uint tone_duration = 100;
    if(repeat) {
        tone_duration = 50;
    }

    /*
    if(btn == 0) {
        if (profile_edit_state->editing == 0) {
            if(profile_edit_state->selected_value > 0) {
                profile_edit_state->selected_value = (profile_edit_state->selected_value - 1) % 6;
            }
            return;
        }

        if(!profile_edit_state->adjust) {
            if(profile_edit_state->editing > 1) {
                profile_edit_state->editing = (profile_edit_state->editing - 1) % 3;
            }
        }

        if(profile_edit_state->adjust) {
            uint selected = profile_edit_state->selected_value - 1;
            uint edit = profile_edit_state->editing - 2;
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
    }

    if(btn == 1) {
        if (profile_edit_state->editing == 0) {
            profile_edit_state->selected_value = (profile_edit_state->selected_value + 1) % 6;
            return;
        }


        if(!profile_edit_state->adjust) {
            profile_edit_state->editing = (profile_edit_state->editing % 3) + 1;
        } 
        
        if(profile_edit_state->adjust) {
            uint selected = profile_edit_state->selected_value - 1;
            uint edit = profile_edit_state->editing - 2;
            uint* value = &(profile_edit_state->current_profile->targets[selected][edit]);

            if (*value < 229) {
                *value = (*value + 1);
                tone(menu_state->_tonegen, NOTE_A3, tone_duration);
            }
        }
    }

    if(btn == 2 && !repeat) {
        if (profile_edit_state->selected_value == 0 && profile_edit_state->editing == 0) {
            tone(menu_state->_tonegen, NOTE_C3, tone_duration);
            nf_menu_change_screen(menu_state, PROFILES_SCREEN_ID);
            return;
        }

        if (profile_edit_state->editing == 0) {
            if(profile_edit_state->selected_value > 0 &&  profile_edit_state->selected_value < 4) {
                profile_edit_state->editing = 1;

                uint selected = profile_edit_state->selected_value - 1;

                if (selected > 0) {
                    uint* value_prev = &(profile_edit_state->current_profile->targets[selected-1][1]);
                    uint* value = &(profile_edit_state->current_profile->targets[selected][1]);

                    if (*value < *value_prev) {
                        *value = *value_prev;
                    }
                }
            
                tone(menu_state->_tonegen, NOTE_A4, tone_duration);
            }
        } else {
            include/nf_menu/nf_profile_edit_screen.hif (profile_edit_state->editing > 1) {
                profile_edit_state->adjust = !profile_edit_state->adjust;

                if (profile_edit_state->adjust) {
                    tone(menu_state->_tonegen, NOTE_A4, tone_duration);
                } else {
                    tone(menu_state->_tonegen, NOTE_C4, tone_duration);
                }
            } else {
                profile_edit_state->editing = 0;
                tone(menu_state->_tonegen, NOTE_C4, tone_duration);
            }
        }
    }
    */
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
    config_edit_state->editing = 0;
    config_edit_state->adjust = false;

    _nf_menu_screen_fn_ptrs_t config_edit_screen_fns = {
        .on_render = nf_config_edit_render,
        .on_btn = nf_config_edit_btn_handler,
        .on_active = NULL
    };
    
    config_edit_screen->fnptrs = config_edit_screen_fns;
    config_edit_screen->next = NULL;


    nf_menu_add_screen(_menu_screens_ptr, config_edit_screen);
}

