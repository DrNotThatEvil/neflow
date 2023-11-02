#ifndef NF_MENU_COMMON_H
#define NF_MENU_COMMON_H

#include "ssd1306.h"

typedef struct nf_menu _nf_menu_t;
typedef struct nf_menu_screen _nf_menu_screen_t;

typedef struct nf_menu_screen_fn_ptrs _nf_menu_screen_fn_ptrs_t;

typedef struct nf_menu_screen_fn_ptrs {
    void (*on_render)(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data);
    void (*on_btn)(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data);
} _nf_menu_screen_fn_ptrs_t;

typedef struct nf_menu_screen {
    uint id;
    void* extra_data;
    _nf_menu_screen_fn_ptrs_t fnptrs;
    _nf_menu_screen_t* next;
} _nf_menu_screen_t;

typedef struct nf_menu {
    ssd1306_t* _disp_ptr;
    _nf_menu_screen_t* menu_screens;
    _nf_menu_screen_t** current_screen;
    uint refresh_ms;
} _nf_menu_t;


#endif