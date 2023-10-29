#include "nf_menu.h"

void profile_select(void* _menu_state);
void main_menu_renderer(void* _menu_state);

void test_select(void* _menu_state)
{
    _nf_menu_t* _menu = (_nf_menu_t*) _menu_state;

    char str[20];
    strcpy(str, _menu->menu_options[_menu->current_menu_option].name);
    sprintf(_menu->menu_options[_menu->current_menu_option].name, "-%s-", str);
}

void profiles_back(void* _menu_state)
{
    _nf_menu_t* _menu = (_nf_menu_t*) _menu_state;
    _menu->current_menu_option = PROFILES;
}

void profiles_select(void* _menu_state)
{
    _nf_menu_t* _menu = (_nf_menu_t*) _menu_state;
   
    _menu->current_menu_option = 3;
}

void profiles_next(void* _menu_state)
{
    _nf_menu_t* _menu = (_nf_menu_t*) _menu_state;
    uint temp_menu_index = (_menu->current_menu_option + 1) % NOPTIONS;
    if (temp_menu_index < 2) {
        temp_menu_index = 3;
    }

    _menu->current_menu_option = temp_menu_index;
}

void profile_back(void* _menu_state)
{
    _nf_menu_t* _menu = (_nf_menu_t*) _menu_state;

    _menu->menu_options[_menu->current_menu_option].render_fn = main_menu_renderer;
    _menu->menu_options[_menu->current_menu_option].back_fn = profiles_back;
    _menu->menu_options[_menu->current_menu_option].select_fn = profile_select;
    _menu->menu_options[_menu->current_menu_option].next_fn = profiles_next;
}

void profile_menu_renderer(void* _menu_state)
{
    _nf_menu_t* _menu = (_nf_menu_t*) _menu_state;

    ssd1306_t* _disp_ptr = _menu->_disp_ptr;
    _nf_menu_profile* nf_mp = (_nf_menu_profile*) _menu->menu_options[_menu->current_menu_option].extra_data;
    nf_profile_t profile = nf_mp->_profiles[(_menu->current_menu_option - MAIN_OPTIONS)];

     if(nf_mp->error_counter > 0) {
        nf_mp->error_counter--;

        if((nf_mp->error_counter % 2) == 0) {
            ssd1306_draw_string(_disp_ptr, 30, 5, 1, "!! ERROR !!");
            ssd1306_draw_string(_disp_ptr, 5, 22, 1, "Profile not");
            ssd1306_draw_string(_disp_ptr, 5, 32, 1, "configured");
        }

        return;
    }

    char str[20];
    sprintf(str, "Profile %d", (_menu->current_menu_option - 2));

    ssd1306_draw_line(_disp_ptr, 6, 2, 6, 9);
    ssd1306_draw_line(_disp_ptr, 5, 3, 5, 8);
    ssd1306_draw_line(_disp_ptr, 4, 4, 4, 7);
    ssd1306_draw_line(_disp_ptr, 3, 5, 3, 6);

    ssd1306_draw_string(_disp_ptr, 15, 2, 1, str);
    ssd1306_draw_line(_disp_ptr, 0, 12, 127, 12);

    if(profile.initialized) {
        if(nf_mp->current_profile_option == 0) {
            sprintf(str, "Run");
        } else if(nf_mp->current_profile_option == 1) {
            sprintf(str, "View");
        } else {
            sprintf(str, "Edit");
        }
    } else {
        if(nf_mp->current_profile_option == 0) {
            sprintf(str, "Run (E)");
        } else if(nf_mp->current_profile_option == 1) {
            sprintf(str, "View (E)");
        } else {
            sprintf(str, "Edit");
        }
    }

    //ssd1306_draw_line(_disp_ptr, 49, 10, 64, 0);
    //ssd1306_draw_line(_disp_ptr, 64, 0, 79, 10);

    ssd1306_draw_string(_disp_ptr, 5, 25, 2, str);

    //ssd1306_draw_line(_disp_ptr, 49, 50, 64, 60);
    //ssd1306_draw_line(_disp_ptr, 64, 60, 79, 50);
}

void edit_profile_renderer(void* _menu_state)
{
    _nf_menu_t* _menu = (_nf_menu_t*) _menu_state;

    ssd1306_t* _disp_ptr = _menu->_disp_ptr;
    _nf_menu_profile* nf_mp = (_nf_menu_profile*) _menu->menu_options[_menu->current_menu_option].extra_data;

    nf_profile_t* profile_ptr = &nf_mp->_profiles[(_menu->current_menu_option - MAIN_OPTIONS)];
    uint* value_ptr = (uint*)((char*) profile_ptr + nf_mp->edit_index[1] * sizeof(uint));


    char config_strings[4][20];
    strcpy(config_strings[0], "Pre-Heat start"); 
    strcpy(config_strings[1], "Pre-Heat end"); 
    strcpy(config_strings[2], "Ramp-up"); 
    strcpy(config_strings[3], "Peak"); 

    /*
    ssd1306_draw_line(_disp_ptr, 6, 2, 6, 9);
    ssd1306_draw_line(_disp_ptr, 5, 3, 5, 8);
    ssd1306_draw_line(_disp_ptr, 4, 4, 4, 7);
    ssd1306_draw_line(_disp_ptr, 3, 5, 3, 6);
    */

    char str[20];
    sprintf(str, "Editing Profile %d", (_menu->current_menu_option - 2));
    ssd1306_draw_string(_disp_ptr, 15, 2, 1, str);
    ssd1306_draw_line(_disp_ptr, 0, 12, 127, 12);

    //ssd1306_draw_line(_disp_ptr, 49, 10, 64, 0);
    //ssd1306_draw_line(_disp_ptr, 64, 0, 79, 10);

    sprintf(str, "%s", config_strings[nf_mp->edit_index[1] % 4]);
    ssd1306_draw_string(_disp_ptr, 5, 25, 1, str);

    if((nf_mp->edit_index[1] == 0) % 2) {
        sprintf(str, "Temprature:  %d (c)", *value_ptr);
    } else {
        sprintf(str, "Target time: %d (s)", *value_ptr);
    }
    ssd1306_draw_string(_disp_ptr, 5, 35, 1, str);

    //ssd1306_draw_line(_disp_ptr, 49, 50, 64, 60);
    //ssd1306_draw_line(_disp_ptr, 64, 60, 79, 50);
}

void edit_profile_select(void* _menu_state)
{
    _nf_menu_t* _menu = (_nf_menu_t*) _menu_state;
    _nf_menu_profile* nf_mp = (_nf_menu_profile*) _menu->menu_options[_menu->current_menu_option].extra_data;
    
    nf_profile_t* profile_ptr = &nf_mp->_profiles[(_menu->current_menu_option - MAIN_OPTIONS)];
    uint* value_ptr = (uint*)((char*) profile_ptr + nf_mp->edit_index[1] * sizeof(uint));


    //nf_mp->edit_index[0] = (nf_mp->edit_index[0] + 1) % 2;
    nf_mp->edit_index[1] = (nf_mp->edit_index[1] + 1);
}

void edit_profile_value_change(void* _menu_state, int change)
{
    _nf_menu_t* _menu = (_nf_menu_t*) _menu_state;
    _nf_menu_profile* nf_mp = (_nf_menu_profile*) _menu->menu_options[_menu->current_menu_option].extra_data;
    
    nf_profile_t* profile_ptr = &nf_mp->_profiles[(_menu->current_menu_option - MAIN_OPTIONS)];
    uint* value_ptr = (uint*)((char*) profile_ptr + nf_mp->edit_index[1] * sizeof(uint));

    *value_ptr = (*value_ptr+change);

    if(nf_mp->edit_index[0] == 0) {
        if(*value_ptr < 1) {
            *value_ptr = 1;
        }

        if(*value_ptr > 230) {
            *value_ptr = 230;
        }
        return;
    }

    if(nf_mp->edit_index[1] > 0) {
        uint* prev_ptr = (uint*)((char*) profile_ptr + (nf_mp->edit_index[1]-1) * sizeof(uint));

        if(*value_ptr < *prev_ptr) {
            *value_ptr = *prev_ptr;
        }
    }
}

void edit_profile_next(void* _menu_state)
{
    edit_profile_value_change(_menu_state, 5);
}

void edit_profile_back(void* _menu_state)
{
    edit_profile_value_change(_menu_state, -5);
}


void profile_next(void* _menu_state)
{
    _nf_menu_t* _menu = (_nf_menu_t*) _menu_state;
    _nf_menu_profile* nf_mp = (_nf_menu_profile*) _menu->menu_options[_menu->current_menu_option].extra_data;
    nf_mp->current_profile_option = (nf_mp->current_profile_option + 1) % 3;
}

void profile_select_option(void* _menu_state)
{
    _nf_menu_t* _menu = (_nf_menu_t*) _menu_state;
    _nf_menu_profile* nf_mp = (_nf_menu_profile*) _menu->menu_options[_menu->current_menu_option].extra_data;
    nf_profile_t profile = nf_mp->_profiles[(_menu->current_menu_option - MAIN_OPTIONS)];

    if(!profile.initialized && nf_mp->current_profile_option < 2) {
        nf_mp->error_counter = 5;
        return;
    }

    if(nf_mp->current_profile_option == 2) {
        _menu->menu_options[_menu->current_menu_option].render_fn = edit_profile_renderer;
        _menu->menu_options[_menu->current_menu_option].select_fn = edit_profile_select;
        _menu->menu_options[_menu->current_menu_option].back_fn = edit_profile_back;
        _menu->menu_options[_menu->current_menu_option].next_fn = edit_profile_next;
        return;
    }
}

void profile_select(void* _menu_state)
{
    _nf_menu_t* _menu = (_nf_menu_t*) _menu_state;

    
    _menu->menu_options[_menu->current_menu_option].render_fn = profile_menu_renderer;
    _menu->menu_options[_menu->current_menu_option].back_fn = profile_back;
    _menu->menu_options[_menu->current_menu_option].select_fn = profile_select_option;
    _menu->menu_options[_menu->current_menu_option].next_fn = profile_next;
}

void main_next(void* _menu_state)
{
    _nf_menu_t* _menu = (_nf_menu_t*) _menu_state;
    _menu->current_menu_option = ((_menu->current_menu_option + 1) % MAIN_OPTIONS);
}

void main_back(void* _menu_state)
{
}

void main_menu_renderer(void* _menu_state)
{
    _nf_menu_t* _menu = (_nf_menu_t*) _menu_state;
    ssd1306_t* _disp_ptr = _menu->_disp_ptr;

    char str[20];
    sprintf(str, "%s", _menu->menu_options[_menu->current_menu_option]);

    ssd1306_draw_line(_disp_ptr, 122, 28, 122, 35);
    ssd1306_draw_line(_disp_ptr, 123, 29, 123, 34);
    ssd1306_draw_line(_disp_ptr, 124, 30, 124, 33);
    ssd1306_draw_line(_disp_ptr, 125, 31, 125, 32);

    ssd1306_draw_string(_disp_ptr, 5, 25, 2, str);

    if(_menu->current_menu_option > 2) {
        
        ssd1306_draw_line(_disp_ptr, 6, 2, 6, 9);
        ssd1306_draw_line(_disp_ptr, 5, 3, 5, 8);
        ssd1306_draw_line(_disp_ptr, 4, 4, 4, 7);
        ssd1306_draw_line(_disp_ptr, 3, 5, 3, 6);

        sprintf(str, "Mains");
        ssd1306_draw_string(_disp_ptr, 15, 2, 1, str);

        ssd1306_draw_line(_disp_ptr, 0, 12, 127, 12);
    }

/*
    ssd1306_draw_pixel(_disp_ptr, 125, 29);
    ssd1306_draw_pixel(_disp_ptr, 125, 30);
    ssd1306_draw_pixel(_disp_ptr, 126, 30);

    ssd1306_draw_line(_disp_ptr, 122, 31, 127, 31);
    ssd1306_draw_line(_disp_ptr, 122, 32, 122, 32);
*/

    //ssd1306_draw_line(_disp_ptr, , 50, 64, 60);
    //ssd1306_draw_line(_disp_ptr, 64, 60, 79, 50);
}

void nf_menu_render(_nf_menu_t* _menu)
{
    _menu->menu_options[_menu->current_menu_option].render_fn((void*)_menu);
}

void nf_menu_init(_nf_menu_t* _menu_state, ssd1306_t* _disp_ptr, nf_profile_t* _profiles)
{
    _menu_state->_disp_ptr = _disp_ptr;
    _menu_state->current_menu_option = 0;
    _menu_state->menu_options = (_nf_menu_option*) malloc(NOPTIONS * sizeof(_nf_menu_option));

    _menu_state->menu_options[PROFILES].name = malloc(sizeof(char) * 20);
    strcpy(_menu_state->menu_options[PROFILES].name, "Profiles");
    _menu_state->menu_options[PROFILES].render_fn = main_menu_renderer;
    _menu_state->menu_options[PROFILES].back_fn = main_back;
    _menu_state->menu_options[PROFILES].select_fn = profiles_select;
    _menu_state->menu_options[PROFILES].next_fn = main_next;
    _menu_state->menu_options[PROFILES].extra_data = (void*)0;

    _menu_state->menu_options[1].name = malloc(sizeof(char) * 20);
    strcpy(_menu_state->menu_options[1].name, "Calibrate");
    _menu_state->menu_options[1].render_fn = main_menu_renderer;
    _menu_state->menu_options[1].back_fn = main_back;
    _menu_state->menu_options[1].select_fn = test_select;
    _menu_state->menu_options[1].next_fn = main_next;
    _menu_state->menu_options[1].extra_data = (void*)0;
    
    _menu_state->menu_options[2].name = malloc(sizeof(char) * 20);
    strcpy(_menu_state->menu_options[2].name, "Config");
    _menu_state->menu_options[2].render_fn = main_menu_renderer;
    _menu_state->menu_options[2].back_fn = main_back;
    _menu_state->menu_options[2].select_fn = test_select;
    _menu_state->menu_options[2].next_fn = main_next;
    _menu_state->menu_options[2].extra_data = (void*)0;

    _nf_menu_profile* _menu_profile_ptr = (_nf_menu_profile*) malloc(sizeof(_nf_menu_profile));
    _menu_profile_ptr->current_profile_option = 0;
    _menu_profile_ptr->error_counter = 0;
    _menu_profile_ptr->edit_index[0] = 0;
    _menu_profile_ptr->edit_index[1] = 0;
    _menu_profile_ptr->_profiles = _profiles;

    for(int i = 3; i < NOPTIONS; i++) 
    {
        _menu_state->menu_options[i].name = malloc(sizeof(char) * 20);
        sprintf(_menu_state->menu_options[i].name, "Profile %d", (i - 2));
        _menu_state->menu_options[i].render_fn = main_menu_renderer;
        _menu_state->menu_options[i].back_fn = profiles_back;
        _menu_state->menu_options[i].select_fn = profile_select;
        _menu_state->menu_options[i].next_fn = profiles_next;
        _menu_state->menu_options[i].extra_data = (void*) _menu_profile_ptr;
    }
}