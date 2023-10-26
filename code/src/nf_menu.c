#include "nf_menu.h"

void test_select(void* _menu_options, uint index)
{
    _nf_menu_option* _options = (_nf_menu_option*) _menu_options;

    char str[20];
    strcpy(str, _options[index].name);
    sprintf(_options[index].name, "-%s-", str);
/*
    char str[20];
    strcpy(str, _options[index]->name);

    sprintf(_options[index]->name, "*%s*", str);
    _options[index]->name[sizeof(str) - 1] = '\0';
*/
    
}

void nf_menu_init(_nf_menu_t* _menu_state)
{
    _menu_state->current_menu_option = 0;
    _menu_state->menu_options = malloc(NOPTIONS * sizeof(_nf_menu_option));

    _menu_state->menu_options[0].name = malloc(sizeof(char) * 20);
    strcpy(_menu_state->menu_options[0].name, "cool");
    _menu_state->menu_options[0].select_fn = test_select;

    _menu_state->menu_options[1].name = malloc(sizeof(char) * 20);
    strcpy(_menu_state->menu_options[0].name, "test");
    _menu_state->menu_options[1].select_fn = test_select;

    _menu_state->menu_options[2].name = malloc(sizeof(char) * 20);
    strcpy(_menu_state->menu_options[0].name, "vet");
    _menu_state->menu_options[2].select_fn = test_select;
}