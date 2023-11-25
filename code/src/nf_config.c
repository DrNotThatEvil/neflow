#include "nf_config.h"

void nf_init(nf_state_t* _state)
{
    _state->_tonegen = malloc(sizeof(tonegenerator_t));
    tone_init(_state->_tonegen, NF_SPKR_PIN);

    _state->_menu_state = NORMAL;
    _state->_reflow_state = NONE;

    for(int i=0; i < 3; i++) {
        _state->_buttons[i].pressed = false;
        _state->_buttons[i].held = false;
        _state->_buttons[i].held_count = 0;
        _state->_buttons[i].released = false;
    }

    _state->_disp_ptr = (ssd1306_t*) malloc(sizeof(ssd1306_t));
    _state->_profiles = (nf_profile_t*) malloc(3 * sizeof(nf_profile_t));
    _state->_memory = (_nf_memory_state_t*) malloc(sizeof(_nf_memory_state_t));

    _state->_menu = malloc(sizeof(_nf_menu_t));
    _state->_disp_ptr->external_vcc = false;
    ssd1306_init(_state->_disp_ptr, 128, 64, 0x3C, i2c1);

    nf_memory_init(_state->_memory);
    nf_profiles_init(_state->_profiles, _state->_memory);
    nf_menu_init(_state->_menu, _state->_disp_ptr, _state->_tonegen, _state->_profiles);
}