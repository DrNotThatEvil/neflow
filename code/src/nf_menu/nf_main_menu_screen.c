#include "nf_menu/nf_main_menu_screen.h"

void nf_main_menu_render(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data)
{
    char temp_str[20];
    sprintf(temp_str, "Temp: %.2f", menu_state->cur_temp);
    ssd1306_draw_string(disp_ptr, 5, 5, 1, temp_str);

    if(menu_state->heater_state == 1) {
        ssd1306_draw_square(disp_ptr, 120, 5, 5, 5);
    }
    
    char str[20];
    uint test = *((uint*)extra_data);
    if(test == PROFILES_INDEX) {
        sprintf(str, "%s", "Profiles\0");
    } else if (test == CALIBRATION_INDEX) {
        sprintf(str, "%s", "Calibration\0");
    } else if (test == CONFIG_INDEX) {
        sprintf(str, "%s", "Config\0");
#if ENABLE_TEST_MENU == 1
    } else if (test == TEST_MENU_INDEX) {
        sprintf(str, "%s", "Test\0");
#endif
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

    if(btn == 1) {
        *data = ((*data) + 1) % MAX_MAIN_MENU_OPTION_INDEX;
        tone(menu_state->_tonegen, NOTE_E4, 100);
        return;
    }

    if(btn == 2) {
        if(*data == PROFILES_INDEX) {
            tone(menu_state->_tonegen, NOTE_C4, 150);
            nf_menu_change_screen(menu_state, PROFILES_SCREEN_ID);
        }

        if(*data == CONFIG_INDEX) {
            tone(menu_state->_tonegen, NOTE_C4, 150);
            nf_menu_change_screen(menu_state, CONFIG_SCREEN_ID);
        }

        if(*data == CALIBRATION_INDEX) {
            //menu_state->_state = MENU_STATE_CALIBRATION;
            nf_menu_change_state(menu_state, MENU_STATE_CALIBRATION);
            nf_menu_change_screen(menu_state, CALIBRATION_SCREEN_ID);
        }
#if ENABLE_TEST_MENU == 1
        if(*data == TEST_MENU_INDEX) {
            tone(menu_state->_tonegen, NOTE_C4, 150);
            nf_menu_change_screen(menu_state, TEST_SCREEN_ID);
        }
#endif
        //if(*data == 1) {
        //    *data = ((*data) + 1) % 2;
        //}
    }
}

void nf_main_menu_init(_nf_menu_t* _menu_state)
{
    // get start of the screens.
    _nf_menu_screen_t** _menu_screens_ptr = &_menu_state->menu_screens;

    _nf_menu_screen_t* main_screen = (_nf_menu_screen_t*) malloc(sizeof(_nf_menu_screen_t));
    main_screen->id = MAIN_MENU_SCREEN_ID;
    main_screen->autoclear = true;
    main_screen->extra_data = malloc(sizeof(uint));

    uint* option = main_screen->extra_data;
    *option = PROFILES_INDEX;

    _nf_menu_screen_fn_ptrs_t main_screen_fns = {
        .on_render = nf_main_menu_render,
        .on_btn = nf_main_menu_btn_handler,
        .on_active = NULL
    };
    
    main_screen->fnptrs = main_screen_fns;
    main_screen->next = NULL;

    nf_menu_add_screen(_menu_screens_ptr, main_screen);

}