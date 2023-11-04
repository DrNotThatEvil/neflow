#include "nf_menu/nf_profile_edit_screen.h"

void draw_edit_item(ssd1306_t* disp_ptr, uint y, bool selected, const char* str)
{
    ssd1306_draw_string(disp_ptr, 5, (y+2), 1, str);

    if(selected) {
        ssd1306_draw_line(disp_ptr, 1, y, 127, y);
        ssd1306_draw_line(disp_ptr, 1, (y+11), 127, (y+11));
    }
}

void nf_profile_edit_render(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data)
{
    _nf_profile_edit_screen_state_t* profile_edit_state = ((_nf_profile_edit_screen_state_t*) extra_data);
    uint selected = profile_edit_state->selected_value;

    if(!profile_edit_state->editing) {
        draw_edit_item(disp_ptr, 0, (selected == 0), "Back");
        draw_edit_item(disp_ptr, 11, (selected == 1), "Pre-heat start");
        draw_edit_item(disp_ptr, 22, (selected == 2), "Pre-Heat end");
        draw_edit_item(disp_ptr, 33, (selected == 3), "Ramp-up");
        draw_edit_item(disp_ptr, 44, (selected == 4), "Peak");
        draw_edit_item(disp_ptr, 55, (selected == 5), "Save");

        return;
    }

    
    draw_edit_item(disp_ptr, 0, (selected == 0), "Back");

}

void nf_profile_edit_btn_handler(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data)
{
    _nf_profile_edit_screen_state_t* profile_edit_state = ((_nf_profile_edit_screen_state_t*) extra_data);

    if(btn == 1) {
        profile_edit_state->selected_value = (profile_edit_state->selected_value + 1) % 5;
    }

    if(btn == 2) {
        if (profile_edit_state->selected_value == 0) {
            return;
        }

        profile_edit_state->editing = true;
    }
}

void nf_profile_edit_menu_init(_nf_menu_t* _menu_state, nf_profile_t* _profiles_ptr)
{
    _nf_menu_screen_t** _menu_screens_ptr = &_menu_state->menu_screens;
    _nf_menu_screen_t* profile_edit_screen = (_nf_menu_screen_t*) malloc(sizeof(_nf_menu_screen_t));

    profile_edit_screen->id = 2;
    profile_edit_screen->extra_data = malloc(sizeof(_nf_profile_edit_screen_state_t));

    _nf_profile_edit_screen_state_t* profile_edit_state = (_nf_profile_edit_screen_state_t*) profile_edit_screen->extra_data;
    profile_edit_state->current_profile = NULL;
    profile_edit_state->selected_value = 0;
    profile_edit_state->editing = false;

    _nf_menu_screen_fn_ptrs_t profile_edit_screen_fns = {
        .on_render = nf_profile_edit_render,
        .on_btn = nf_profile_edit_btn_handler
    };
    
    profile_edit_screen->fnptrs = profile_edit_screen_fns;
    profile_edit_screen->next = NULL;


    nf_menu_add_screen(_menu_screens_ptr, profile_edit_screen);
}

