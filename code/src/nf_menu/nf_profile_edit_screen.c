#include "nf_menu/nf_profile_edit_screen.h"


void nf_profile_edit_render(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data)
{
    _nf_profile_edit_screen_state_t* profile_edit_state = ((_nf_profile_edit_screen_state_t*) extra_data);
    uint selected = profile_edit_state->selected_value;

    if(profile_edit_state->editing == 0) {
        draw_edit_item(disp_ptr, 0, (selected == 0), "Back");
        draw_edit_item(disp_ptr, 11, (selected == 1), "Pre-heat start");
        draw_edit_item(disp_ptr, 22, (selected == 2), "Pre-Heat end");
        draw_edit_item(disp_ptr, 33, (selected == 3), "Ramp-up");
        draw_edit_item(disp_ptr, 44, (selected == 4), "Peak");
        draw_edit_item(disp_ptr, 53, (selected == 5), "Save");

        return;
    }

    draw_edit_item(disp_ptr, 0, (profile_edit_state->editing == 1 && !profile_edit_state->adjust), "Back");
    draw_edit_item(disp_ptr, 11, (profile_edit_state->editing == 2 && !profile_edit_state->adjust), "Target temp:");

    char str[20];
    sprintf(str, "%d", (profile_edit_state->current_profile->targets[(selected - 1)][0]));
    draw_edit_value(disp_ptr, 22, (profile_edit_state->editing == 2 && profile_edit_state->adjust), str);
    
    
    draw_edit_item(disp_ptr, 34, (profile_edit_state->editing == 3 && !profile_edit_state->adjust), "Target time:");
    sprintf(str, "%d", (profile_edit_state->current_profile->targets[(selected - 1)][1]));
    draw_edit_value(disp_ptr, 45, (profile_edit_state->editing == 3 && profile_edit_state->adjust), str);
}

void nf_profile_edit_on_active_handler(_nf_menu_t* menu_state, void* extra_data, void* on_active_extra_data)
{
    _nf_profile_edit_screen_state_t* profile_edit_state = ((_nf_profile_edit_screen_state_t*) extra_data);
    _nf_profile_t* cur_profile = (_nf_profile_t*) on_active_extra_data;

    profile_edit_state->current_profile = cur_profile;
}

void nf_profile_edit_btn_handler(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data)
{
    _nf_profile_edit_screen_state_t* profile_edit_state = ((_nf_profile_edit_screen_state_t*) extra_data);
    uint tone_duration = 100;
    if(repeat) {
        tone_duration = 50;
    }

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

            /*
            TODO:
                It's that time again where i have to stop myself from going to deep into the night but progress!
                The edit menu interaction is now able to get the current profile value
                edit it and such (there will probs be some quirks it's good enough).

                Next time:
                The saving of the profile to the flash of the RP2040 along with the reading of what's already stored there
                next to this the Config screen needs to be added since 'profiles' and 'config' are both saved to the flash.
                But i bet the config screen can take a lot of inspiration of this screen making that simpler.

                Next Next time:
                I think that you won't have time to do anything more then the 'memory/config' screen next time
                but for the next next time you can start with the 'TEMP' system, see the draw.io for logic.
                knocking out the memory system first is smart tho cause then you can make testing quicker since you would
                not have to configure the pid and profiles all the time.
            */
        } else {
            if (profile_edit_state->editing > 1) {
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
}

void nf_profile_edit_menu_init(_nf_menu_t* _menu_state, _nf_profile_t* _profiles_ptr)
{
    _nf_menu_screen_t** _menu_screens_ptr = &_menu_state->menu_screens;
    _nf_menu_screen_t* profile_edit_screen = (_nf_menu_screen_t*) malloc(sizeof(_nf_menu_screen_t));

    profile_edit_screen->id = PROFILE_EDIT_SCREEN_ID;
    profile_edit_screen->extra_data = malloc(sizeof(_nf_profile_edit_screen_state_t));

    _nf_profile_edit_screen_state_t* profile_edit_state = (_nf_profile_edit_screen_state_t*) profile_edit_screen->extra_data;
    profile_edit_state->current_profile = NULL;
    profile_edit_state->selected_value = 0;
    profile_edit_state->editing = 0;
    profile_edit_state->adjust = false;

    _nf_menu_screen_fn_ptrs_t profile_edit_screen_fns = {
        .on_render = nf_profile_edit_render,
        .on_btn = nf_profile_edit_btn_handler,
        .on_active = nf_profile_edit_on_active_handler
    };
    
    profile_edit_screen->fnptrs = profile_edit_screen_fns;
    profile_edit_screen->next = NULL;


    nf_menu_add_screen(_menu_screens_ptr, profile_edit_screen);
}

