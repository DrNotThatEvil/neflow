#ifndef NF_MENU_H
#define NF_MENU_H

#include <pico/types.h>

#define NOPTIONS 3

typedef struct {
    char* name;
} _nf_menu_option;

typedef struct {
    _nf_menu_option* menu_options;
    uint current_menu_option; 
} _nf_menu_t;

void nf_menu_init(_nf_menu_t* _menu_state);

#endif