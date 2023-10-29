#include "nf_config.h"

void nf_init(nf_state_t* _state)
{
    _state->_menu_state = NORMAL;
    _state->_reflow_state = NONE;
    _state->_disp_ptr = (ssd1306_t*) malloc(sizeof(ssd1306_t));
    _state->_profiles = (nf_profile_t*) malloc(3 * sizeof(nf_profile_t));

    _state->_menu = malloc(sizeof(_nf_menu_t));
    _state->_disp_ptr->external_vcc = false;
    ssd1306_init(_state->_disp_ptr, 128, 64, 0x3C, i2c1);

    nf_profiles_init(_state->_profiles);
    nf_menu_init(_state->_menu, _state->_disp_ptr, _state->_profiles);
}