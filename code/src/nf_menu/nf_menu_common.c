#include "nf_menu/nf_menu_common.h"

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

void nf_menu_change_screen(_nf_menu_t* _menu, uint screen_id)
{
    _nf_menu_screen_t** current = &(_menu->menu_screens);

    while (*current != NULL) {
        if ((*current)->id == screen_id) {
            _menu->current_screen = current;           

            return;
        }
        current = &((_menu->menu_screens)->next);
    }
}

void draw_next_arrow(ssd1306_t* disp_ptr)
{
    ssd1306_draw_line(disp_ptr, 122, 28, 122, 35);
    ssd1306_draw_line(disp_ptr, 123, 29, 123, 34);
    ssd1306_draw_line(disp_ptr, 124, 30, 124, 33);
    ssd1306_draw_line(disp_ptr, 125, 31, 125, 32);
}