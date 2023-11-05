#include "nf_menu/nf_profile_edit_screen.h"

void draw_edit_item(ssd1306_t* disp_ptr, uint y, bool selected, const char* str)
{
    ssd1306_draw_string(disp_ptr, 5, (y+2), 1, str);

    if(selected) {
        ssd1306_draw_line(disp_ptr, 1, y, 127, y);
        ssd1306_draw_line(disp_ptr, 1, (y+11), 127, (y+11));
    }
}

void draw_edit_value(ssd1306_t* disp_ptr, uint y, bool selected, const char* str)
{
    ssd1306_draw_string(disp_ptr, 55, (y+4), 1, str);

    if(selected) {
        ssd1306_draw_line(disp_ptr, 44, (y+6), 44, (y+7));
        ssd1306_draw_line(disp_ptr, 45, (y+5), 45, (y+8));
        ssd1306_draw_line(disp_ptr, 46, (y+4), 46, (y+9));
        ssd1306_draw_line(disp_ptr, 47, (y+3), 47, (y+10));
        ssd1306_draw_line(disp_ptr, 48, (y+2), 48, (y+11));


        ssd1306_draw_line(disp_ptr, 84, (y+2), 84, (y+11));
        ssd1306_draw_line(disp_ptr, 85, (y+3), 85, (y+10));
        ssd1306_draw_line(disp_ptr, 86, (y+4), 86, (y+9));
        ssd1306_draw_line(disp_ptr, 87, (y+5), 87, (y+8));
        ssd1306_draw_line(disp_ptr, 88, (y+6), 88, (y+7));
        //ssd1306_draw_line(disp_ptr, 1, (y+11), 127, (y+11));
    }
}

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
        draw_edit_item(disp_ptr, 54, (selected == 5), "Save");

        return;
    }

    draw_edit_item(disp_ptr, 0, (profile_edit_state->editing == 1 && !profile_edit_state->adjust), "Back");
    draw_edit_item(disp_ptr, 11, (profile_edit_state->editing == 2 && !profile_edit_state->adjust), "Target temp:");

    char str[20];
    sprintf(str, "%d", 100);
    draw_edit_value(disp_ptr, 22, (profile_edit_state->editing == 2 && profile_edit_state->adjust), str);
    
    
    draw_edit_item(disp_ptr, 34, (profile_edit_state->editing == 3 && !profile_edit_state->adjust), "Target time:");
    sprintf(str, "%d", 100);
    draw_edit_value(disp_ptr, 45, (profile_edit_state->editing == 3 && profile_edit_state->adjust), str);

}

void nf_profile_edit_btn_handler(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data)
{
    _nf_profile_edit_screen_state_t* profile_edit_state = ((_nf_profile_edit_screen_state_t*) extra_data);

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
    }

    if(btn == 1) {
        if (profile_edit_state->editing == 0) {
            profile_edit_state->selected_value = (profile_edit_state->selected_value + 1) % 6;
            return;
        }


        if(!profile_edit_state->adjust) {
            profile_edit_state->editing = (profile_edit_state->editing % 3) + 1;
        }
    }

    if(btn == 2) {
        if (profile_edit_state->selected_value == 0 && profile_edit_state->editing == 0) {
            return;
        }

        if (profile_edit_state->editing == 0) {
            if(profile_edit_state->selected_value > 0) {
                profile_edit_state->editing = 1;
            }
        } else {
            if (profile_edit_state->editing > 1) {
                profile_edit_state->adjust = !profile_edit_state->adjust;
            } else {
                profile_edit_state->editing = 0;
            }
        }
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
    profile_edit_state->editing = 0;
    profile_edit_state->adjust = false;

    _nf_menu_screen_fn_ptrs_t profile_edit_screen_fns = {
        .on_render = nf_profile_edit_render,
        .on_btn = nf_profile_edit_btn_handler
    };
    
    profile_edit_screen->fnptrs = profile_edit_screen_fns;
    profile_edit_screen->next = NULL;


    nf_menu_add_screen(_menu_screens_ptr, profile_edit_screen);
}

