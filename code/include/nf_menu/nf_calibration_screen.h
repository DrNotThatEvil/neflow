#ifndef NF_CALIBRATION_SCREEN_H
#define NF_CALIBRATION_SCREEN_H

#include "nf_menu/nf_menu_common.h"
#include "nf_menu/nf_graph.h"

#define NF_GRAPH_ZERO_X 20
#define NF_GRAPH_MAX_X 120

#define NF_GRAPH_ZERO_Y 44
#define NF_GRAPH_MAX_Y 0

#define NF_AVG_TIMEOUT 1000
//#define NF_AVG_TIMEOUT 60000
//#define NF_SAMPLE_TIMEOUT 10000
#define NF_SAMPLE_TIMEOUT 250

typedef struct nf_calibration_screen_state {
    uint8_t _sample_count;
    double _sum;
    uint8_t _avg_count;
    bool _background_drawn;
    uint32_t _last_line_end_x;
    uint32_t _last_line_end_y;
    absolute_time_t _sample_timeout;
    absolute_time_t _avg_timeout;
} _nf_calibration_screen_state_t;

void nf_calibration_screen_init(_nf_menu_t* _menu_state);
void _nf_gen_calibration_screen_state(_nf_calibration_screen_state_t* _cali_state);

void nf_calibration_screen_render(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data);
void nf_calibration_screen_btn_handler(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data);
void nf_calibration_screen_on_active(_nf_menu_t* _menu_state, void* extra_data, void* on_active_extra_data);

#endif