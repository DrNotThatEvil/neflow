#ifndef NF_CONFIG_EDIT_SCREEN_H
#define NF_CONFIG_EDIT_SCREEN_H

#include "nf_memory.h"
#include "nf_menu/nf_menu_common.h"

typedef struct nf_config_edit_screen_state
{
    _nf_memory_state_t *_memory_state;
    uint selected_value;
    uint selected_set;
    uint editing;
    bool adjust;
    uint change;
} _nf_config_edit_screen_state_t;

void nf_config_edit_render(_nf_menu_t *menu_state, ssd1306_t *disp_ptr,
                           void *extra_data);
void nf_config_edit_on_active_handler(_nf_menu_t *menu_state, void *extra_data,
                                      void *on_active_extra_data);
void nf_config_edit_btn_handler(_nf_menu_t *menu_state, uint btn, bool repeat,
                                void *extra_data);
void nf_config_edit_menu_init(_nf_menu_t *_menu_state,
                              _nf_memory_state_t *_memory_state_ptr);
void nf_config_edit_cb(void *_nf_menu_vptr);

#endif
