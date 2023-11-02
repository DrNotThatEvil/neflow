#include "nf_menu/nf_profiles_screen.h"

void nf_profiles_render(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data)
{
    char str[20];
    _nf_profiles_menu_state_t* profiles_state = ((_nf_profiles_menu_state_t*) extra_data);

    if(!profiles_state->selected) {
        sprintf(str, "Profile %d\0", (profiles_state->selected_profile+1));

        draw_next_arrow(disp_ptr);
        ssd1306_draw_string(disp_ptr, 5, 25, 2, str);
        
        sprintf(str, "not configured!\0");
        ssd1306_draw_string(disp_ptr, 5, 40, 1, str);
        return;
    }

    if(profiles_state->selected_option == 0) {
        sprintf(str, "Run", (profiles_state->selected_profile+1));
    } else if(profiles_state->selected_option == 1) {
        sprintf(str, "View", (profiles_state->selected_profile+1));
    } else {
        sprintf(str, "Edit", (profiles_state->selected_profile+1));
    }

    draw_next_arrow(disp_ptr);
    ssd1306_draw_string(disp_ptr, 5, 25, 2, str);
    
    sprintf(str, "not configured!");
    ssd1306_draw_string(disp_ptr, 5, 40, 1, str);
}

void nf_profiles_btn_handler(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data)
{
    _nf_profiles_menu_state_t* profiles_state = ((_nf_profiles_menu_state_t*) extra_data);

    if(!profiles_state->selected) {
        if(btn == 0) {
            nf_menu_change_screen(menu_state, 0);
            return;
        }

        if(btn == 1) {
            profiles_state->selected_profile = ((profiles_state->selected_profile) + 1) % 3;
            return;
        } 

        if(btn == 2) {
            profiles_state->selected = true;
            return;
        }
    }

    if(profiles_state->selected) {
        if(btn == 0) {
            profiles_state->selected = false;
            return;
        }

        if(btn == 1) {
            profiles_state->selected_option = ((profiles_state->selected_option) + 1) % 3;
            return;
        } 
    }
}

void nf_profiles_menu_init(_nf_menu_t* _menu_state, nf_profile_t* _profiles)
{
    // get start of the screens.
    _nf_menu_screen_t** _menu_screens_ptr = &_menu_state->menu_screens;
    _nf_menu_screen_t* profiles_screen = (_nf_menu_screen_t*) malloc(sizeof(_nf_menu_screen_t));

    profiles_screen->id = 1;
    profiles_screen->extra_data = malloc(sizeof(_nf_profiles_menu_state_t));

    _nf_profiles_menu_state_t* profile_menu_state = (_nf_profiles_menu_state_t*) profiles_screen->extra_data;
    profile_menu_state->selected_profile = 0;
    profile_menu_state->selected_option = 0;
    profile_menu_state->selected = false;
    profile_menu_state->profiles_ptr = _profiles;

    _nf_menu_screen_fn_ptrs_t profiles_screen_fns = {
        .on_render = nf_profiles_render,
        .on_btn = nf_profiles_btn_handler
    };
    
    profiles_screen->fnptrs = profiles_screen_fns;
    profiles_screen->next = NULL;


    nf_menu_add_screen(_menu_screens_ptr, profiles_screen);
}
