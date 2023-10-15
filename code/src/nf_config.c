#include "nf_config.h"

void nf_init(nf_state_t* _state)
{
    _state->_menu_state = NORMAL;
    _state->_reflow_state = NONE;
}