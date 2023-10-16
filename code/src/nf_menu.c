#include "nf_menu.h"

void nf_menu_init(_nf_menu_t* _menu_state)
{
    _menu_state->current_menu_option = 0;
    _menu_state->menu_options = malloc(NOPTIONS * sizeof(_nf_menu_option));

    _menu_state->menu_options[0].name = "Test";
    _menu_state->menu_options[1].name = "Fuck";
    _menu_state->menu_options[2].name = "Blap";
}