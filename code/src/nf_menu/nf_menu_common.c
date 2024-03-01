#include "nf_menu/nf_menu_common.h"

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

void nf_menu_add_screen(_nf_menu_screen_t** _menu_screens_ptr, _nf_menu_screen_t* _new_screen)
{
    // If the list is empty, make the new node the head of the list
    if (*_menu_screens_ptr == NULL) {
        *_menu_screens_ptr = _new_screen;
    } else {
        // Find the last node and update its 'next' pointer
        _nf_menu_screen_t* current = *_menu_screens_ptr;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = _new_screen;
    }
}

void nf_menu_change_screen_with_data(_nf_menu_t* _menu, uint screen_id, void* on_active_extra_data)
{
    _nf_menu_screen_t** current = &(_menu->menu_screens);

    while (*current != NULL) {
        if ((*current)->id == screen_id)
        {
            _menu->current_screen = current;
            if((*_menu->current_screen)->fnptrs.on_active != NULL)
            {
                (*_menu->current_screen)->fnptrs.on_active(_menu, (*_menu->current_screen)->extra_data, on_active_extra_data);
            }

            return;
        }
        current = &((*current)->next);
    }
}

void nf_menu_change_screen(_nf_menu_t* _menu, uint screen_id)
{
    nf_menu_change_screen_with_data(_menu, screen_id, NULL);
}

void nf_menu_change_state(_nf_menu_t* _menu, _nf_menu_state_t state)
{
    _menu->_state = state;
    _nf_thread_msg menu_state_change_msg = {
        .msg_type = MENU_STATE_CHANGE_MSG_TYPE,
        .simple_msg_value = ((uint) state)
    };

    queue_add_blocking(&_menu->tempsys_msg_q, &menu_state_change_msg);
}

void nf_set_profile(_nf_menu_t* _menu, void* profile)
{
    _nf_thread_msg profile_msg = {
        .msg_type = MENU_SET_PROFILE_MSG_TYPE,
        .value_ptr = profile
    };

    queue_add_blocking(&_menu->tempsys_msg_q, &profile_msg);
}

void draw_prev_section(ssd1306_t* disp_ptr, const char* str)
{
    ssd1306_draw_pixel(disp_ptr, 2, 3);
    ssd1306_draw_line(disp_ptr, 3, 2, 3, 4);
    ssd1306_draw_line(disp_ptr, 4, 1, 4, 5);

    ssd1306_draw_string(disp_ptr, 8, 0, 1, str);


    ssd1306_draw_line(disp_ptr, 0, 9, 128, 9);
}

void draw_next_arrow(ssd1306_t* disp_ptr)
{
    ssd1306_draw_line(disp_ptr, 122, 28, 122, 35);
    ssd1306_draw_line(disp_ptr, 123, 29, 123, 34);
    ssd1306_draw_line(disp_ptr, 124, 30, 124, 33);
    ssd1306_draw_line(disp_ptr, 125, 31, 125, 32);
}