#include "nf_menu/nf_main_menu.h"

void nf_main_menu_render(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data)
{
    char str[20];
    uint test = *((uint*)extra_data);

    if(test == 0) {
        sprintf(str, "%s", "test\0");
    } else {
        sprintf(str, "%s", "biem\0");
    }

    ssd1306_draw_line(disp_ptr, 122, 28, 122, 35);
    ssd1306_draw_line(disp_ptr, 123, 29, 123, 34);
    ssd1306_draw_line(disp_ptr, 124, 30, 124, 33);
    ssd1306_draw_line(disp_ptr, 125, 31, 125, 32);

    ssd1306_draw_string(disp_ptr, 5, 25, 2, str);
}

void nf_main_menu_btn_handler(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data)
{
    uint* data = (uint*) extra_data;
    *data = 1;
}

void nf_main_menu_init(_nf_menu_t* _menu_state)
{
    // get start of the screens.
    _nf_menu_screen_t** _menu_screens_ptr = &_menu_state->menu_screens;
    _nf_menu_screen_t* main_screen = (_nf_menu_screen_t*) malloc(sizeof(_nf_menu_screen_t));

    main_screen->id = 0;
    main_screen->extra_data = malloc(sizeof(uint));
    uint* data = main_screen->extra_data;
    *data = 0;

    _nf_menu_screen_fn_ptrs_t main_screen_fns = {
        .on_render = nf_main_menu_render,
        .on_btn = nf_main_menu_btn_handler
    };
    
    main_screen->fnptrs = main_screen_fns;
    main_screen->next = NULL;

    // If the list is empty, make the new node the head of the list
    if (*_menu_screens_ptr == NULL) {
        *_menu_screens_ptr = main_screen;
    } else {
        // Find the last node and update its 'next' pointer
        _nf_menu_screen_t* current = *_menu_screens_ptr;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = main_screen;
    }

}