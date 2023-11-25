#include "nf_menu.h"

void nf_menu_btn_handler(_nf_menu_t* _menu, uint btn, bool repeat)
{
    //_menu->menu_options[_menu->current_menu_option].render_fn((void*)_menu, false);
    (*_menu->current_screen)->fnptrs.on_btn(_menu, btn, repeat, (*_menu->current_screen)->extra_data);
}

void nf_menu_render(_nf_menu_t* _menu)
{
    //_menu->menu_options[_menu->current_menu_option].render_fn((void*)_menu, false);
    (*_menu->current_screen)->fnptrs.on_render(_menu, _menu->_disp_ptr, (*_menu->current_screen)->extra_data);
}

void nf_menu_init(_nf_menu_t* _menu_state, ssd1306_t* _disp_ptr, tonegenerator_t* _tonegen, nf_profile_t* _profiles)
{
    _menu_state->_disp_ptr = _disp_ptr;
    _menu_state->_tonegen = _tonegen;
    _menu_state->refresh_ms = 5;
    _menu_state->menu_screens = NULL; 

    nf_main_menu_init(_menu_state);
    _menu_state->current_screen = &(_menu_state->menu_screens);

    nf_test_menu_init(_menu_state);
    nf_profiles_menu_init(_menu_state, _profiles);
    nf_profile_edit_menu_init(_menu_state, _profiles);
}