#ifndef NF_MENU_H
#define NF_MENU_H

typedef struct {
    char* name;
} _nf_menu_option;

typedef struct {
    _nf_menu_option* current_option;
} _nf_menu_t;

void nf_menu_init(_nf_menu_t* _menu_state);

#endif