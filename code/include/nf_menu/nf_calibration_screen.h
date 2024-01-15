#ifndef NF_CALIBRATION_SCREEN_H
#define NF_CALIBRATION_SCREEN_H

#include "nf_menu/nf_menu_common.h"
#include "nf_menu/nf_graph.h"

#define NF_GRAPH_ZERO_X 20
#define NF_GRAPH_MAX_X 120

#define NF_GRAPH_ZERO_Y 44

void nf_calibration_screen_init(_nf_menu_t* _menu_state);
void nf_calibration_screen_render(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data);
void nf_calibration_screen_btn_handler(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data);

#endif