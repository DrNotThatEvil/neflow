#include "nf_menu/nf_profile_run_screen.h"

const int notes[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_G4, NOTE_A4}; 

void nf_profile_run_render(_nf_menu_t* _menu_state, ssd1306_t* disp_ptr, void* extra_data)
{
    _nf_profile_run_screen_state_t* profile_run_state = ((_nf_profile_run_screen_state_t*) extra_data);
    if(!profile_run_state->_running)
    {
        //ssd1306_draw_string(disp_ptr, 5, 25, 1, "Start Reflow");
        ssd1306_draw_string(_menu_state->_disp_ptr, 2, 5, 1, "Staring reflow");
        ssd1306_draw_string(_menu_state->_disp_ptr, 2, 15, 1, "Please remain near!");
        ssd1306_draw_string(_menu_state->_disp_ptr, 2, 35, 1, "Open door when asked.");
        ssd1306_draw_string(_menu_state->_disp_ptr, 20, 50, 1, "(Press enter)");

        return;
    }

    _nf_graph_state_t* _graph = profile_run_state->_graph;
    ssd1306_clear(disp_ptr);
    if(_menu_state->_state == MENU_STATE_FINISHED && profile_run_state->_finished_door_state < 5)
    {
        if(profile_run_state->_finished_door_state < 3) 
        {
            ssd1306_draw_string(_menu_state->_disp_ptr, 5, 10, 1, "!! Finished !!");
            ssd1306_draw_string(_menu_state->_disp_ptr, 5, 35, 1, "Open door!");
            ssd1306_draw_string(_menu_state->_disp_ptr, 5, 50, 1, "(Press enter after)");
        }

        if(get_absolute_time()._private_us_since_boot > profile_run_state->_beep_timeout._private_us_since_boot)
        {
            tone(_menu_state->_tonegen, notes[profile_run_state->_finished_door_state], 150);

            profile_run_state->_beep_timeout = make_timeout_time_ms(NF_PROFILE_RUN_BEEP_TIMEOUT);
            profile_run_state->_finished_door_state = (profile_run_state->_finished_door_state + 1) % 5;
        }

        return;
    }

    nf_graph_render(_menu_state, disp_ptr, _graph);
    for (uint8_t i = 0; i < PROFILE_TARGETS_SIZE; i++)
    {
        double l = ((((double)profile_run_state->current_profile->targets[i][1]) / _graph->_sec_high_value) * (NF_GRAPH_MAX_X - NF_GRAPH_ZERO_X));

        for(uint8_t y = NF_GRAPH_ZERO_Y; y > NF_GRAPH_MAX_Y; y -= 10)
        {
            ssd1306_draw_line(disp_ptr,
                l + NF_GRAPH_ZERO_X,
                y,
                l + NF_GRAPH_ZERO_X,
                y - 5
            );
        }
    }
}

void nf_profile_run_on_active_handler(_nf_menu_t* _menu_state, void* extra_data, void* on_active_extra_data)
{
    _nf_profile_run_screen_state_t* profile_run_state = ((_nf_profile_run_screen_state_t*) extra_data);
    _nf_profile_t* cur_profile = (_nf_profile_t*) on_active_extra_data;

    profile_run_state->current_profile = cur_profile;
    profile_run_state->_running = false;
    profile_run_state->_finished_door_state = 0;
    profile_run_state->_beep_timeout = make_timeout_time_ms(NF_PROFILE_RUN_BEEP_TIMEOUT);
}

void nf_profile_run_btn_handler(_nf_menu_t* _menu_state, uint btn, bool repeat, void* extra_data)
{
    _nf_profile_run_screen_state_t* profile_run_state = ((_nf_profile_run_screen_state_t*) extra_data);
    if(btn == 2)
    {
        if(!profile_run_state->_running)
        {
            tone(_menu_state->_tonegen, NOTE_C4, 150);
            _nf_graph_state_t* _graph = profile_run_state->_graph;
            _graph->_sample_count = 0;
            _graph->_sum = 0.0;
            _graph->_avg_index = 0;

            _graph->_temp_high_value = 100;
            _graph->_sec_high_value = 100;

            for(uint8_t i = 0; i < NF_GRAPH_NUM_SEGMENTS; i++)
            {
                _graph->_averages[i] = 0.0;
            }

            _graph->_sample_timeout = make_timeout_time_ms(NF_SAMPLE_TIMEOUT);

            profile_run_state->_running = true;
            profile_run_state->_finished_door_state = 0;
            profile_run_state->_beep_timeout = make_timeout_time_ms(NF_PROFILE_RUN_BEEP_TIMEOUT);

            nf_menu_change_state(_menu_state, MENU_STATE_REFLOW);
        }

        if(_menu_state->_state == MENU_STATE_FINISHED && profile_run_state->_running && profile_run_state->_finished_door_state < 5) 
        {
            tone(_menu_state->_tonegen, NOTE_C4, 150);
            profile_run_state->_finished_door_state = 5;
            return;
        }
    }

    if(btn == 0)
    {
        if(!profile_run_state->_running || 
            (_menu_state->_state == MENU_STATE_FINISHED && profile_run_state->_running && profile_run_state->_finished_door_state == 5 && _menu_state->cur_temp < 50.0))
        {
            if(_menu_state->_state == MENU_STATE_FINISHED && profile_run_state->_running && profile_run_state->_finished_door_state == 5)
            {
                tone(_menu_state->_tonegen, NOTE_A2, 150);
            }
            else
            {
                tone(_menu_state->_tonegen, NOTE_B4, 150);
            }
            nf_menu_change_screen(_menu_state, PROFILES_SCREEN_ID);
            return;
        }


        tone(_menu_state->_tonegen, NOTE_B4, 150);
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
    profile_run_state->_finished_door_state = 0;
    
    _nf_graph_state_t* _graph = (_nf_graph_state_t*) malloc(sizeof(_nf_graph_state_t));
    _graph->_sample_count = 0;
    _graph->_sum = 0.0;
    _graph->_avg_index = 0;

    _graph->_temp_high_value = 50;
    _graph->_sec_high_value = 100;

    for(uint8_t i = 0; i < NF_GRAPH_NUM_SEGMENTS; i++)
    {
        _graph->_averages[i] = 0.0;
    }

    _graph->_sample_timeout = make_timeout_time_ms(NF_SAMPLE_TIMEOUT);
    profile_run_state->_graph = _graph;
    profile_run_state->_beep_timeout = make_timeout_time_ms(NF_PROFILE_RUN_BEEP_TIMEOUT);

    _nf_menu_screen_fn_ptrs_t profile_run_screen_fns = {
        .on_render = nf_profile_run_render,
        .on_btn = nf_profile_run_btn_handler,
        .on_active = nf_profile_run_on_active_handler
    };
    
    profile_run_screen->fnptrs = profile_run_screen_fns;
    profile_run_screen->next = NULL;


    nf_menu_add_screen(_menu_screens_ptr, profile_run_screen);
}
