#include "nf_config.h"

void nf_init(nf_state_t* _state)
{
    _state->_tonegen = malloc(sizeof(tonegenerator_t));

    tone_init(_state->_tonegen, NF_SPKR_PIN);

    _state->_reflow_state = NONE;
    _state->_profile_state = (_nf_profile_state_t*) malloc(sizeof(_nf_profile_state_t));
    //_state->_profiles = (_nf_profile_t*) malloc(NUMBER_OF_PROFILES * sizeof(_nf_profile_t));
    _state->_memory = (_nf_memory_state_t*) malloc(sizeof(_nf_memory_state_t));
    _state->_tempsys = (_nf_tempsys_t*) malloc(sizeof(_nf_tempsys_t));

    _state->_menu = malloc(sizeof(_nf_menu_t));
    _state->_temploop_timer = make_timeout_time_ms(100);

    nf_memory_init(_state->_memory);
    nf_profiles_init(_state->_profile_state, _state->_memory);

    nf_tempsys_init(_state->_tempsys, _state->_memory);
    nf_menu_init(_state->_menu, _state->_tonegen, _state->_profile_state, _state->_memory, _state->_tempsys);
}

void nf_tempsys_loop(nf_state_t* _nf_state)
{
    if(get_absolute_time()._private_us_since_boot < _nf_state->_temploop_timer._private_us_since_boot)
    {
        return;
    }

    _nf_state->_temploop_timer = make_timeout_time_ms(100);
    uint menu_state = nf_menu_get_menu_state(_nf_state->_menu);
    nf_tempsys_set_state(_nf_state->_tempsys, menu_state);
    nf_tempsys_update(_nf_state->_tempsys);
}