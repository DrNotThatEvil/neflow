#include "nf_menu/nf_profile_edit_screen.h"

void nf_profile_run_render(_nf_menu_t* _menu_state, ssd1306_t* disp_ptr, void* extra_data)
{
    _nf_profile_run_screen_state_t* profile_run_state = ((_nf_profile_run_screen_state_t*) extra_data);
    if(!profile_run_state->_running)
    {
        ssd1306_draw_string(disp_ptr, 5, 25, 1, "Start Reflow");
        return;
    }


    ssd1306_draw_string(disp_ptr, 5, 25, 2, "RUNNING");
}

void nf_profile_run_on_active_handler(_nf_menu_t* _menu_state, void* extra_data, void* on_active_extra_data)
{
    _nf_profile_run_screen_state_t* profile_run_state = ((_nf_profile_run_screen_state_t*) extra_data);
    _nf_profile_t* cur_profile = (_nf_profile_t*) on_active_extra_data;

    profile_run_state->current_profile = cur_profile;
    profile_run_state->_running = false;
}

void nf_profile_run_btn_handler(_nf_menu_t* _menu_state, uint btn, bool repeat, void* extra_data)
{
    _nf_profile_run_screen_state_t* profile_run_state = ((_nf_profile_run_screen_state_t*) extra_data);
    if(btn == 2)
    {
        if(!profile_run_state->_running)
        {
            nf_menu_change_state(_menu_state, MENU_STATE_REFLOW);
            tone(_menu_state->_tonegen, NOTE_C4, 150);
            profile_run_state->_running = true;
        }
    }
}

void nf_profile_run_screen_init(_nf_menu_t* _menu_state, _nf_profile_state_t* _profile_state)
{
    _nf_menu_screen_t** _menu_screens_ptr = &_menu_state->menu_screens;
    _nf_menu_screen_t* profile_run_screen = (_nf_menu_screen_t*) malloc(sizeof(_nf_menu_screen_t));

    profile_run_screen->id = PROFILE_RUN_SCREEN_ID;
    profile_run_screen->autoclear = true;
    profile_run_screen->extra_data = malloc(sizeof(_nf_profile_run_screen_state_t));

    _nf_profile_run_screen_state_t* profile_run_state = (_nf_profile_run_screen_state_t*) profile_run_screen->extra_data;
    profile_run_state->current_profile = NULL;
    profile_run_state->_profile_state = _profile_state;
    profile_run_state->_running = false;

    _nf_menu_screen_fn_ptrs_t profile_run_screen_fns = {
        .on_render = nf_profile_run_render,
        .on_btn = nf_profile_run_btn_handler,
        .on_active = nf_profile_run_on_active_handler
    };
    
    profile_run_screen->fnptrs = profile_run_screen_fns;
    profile_run_screen->next = NULL;


    nf_menu_add_screen(_menu_screens_ptr, profile_run_screen);
}

