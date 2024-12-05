#include "nf_menu/nf_calibration_screen.h"

/*
 * TODO (DrNotThatEvil, 2024-01-15, 22:16):
 * Well well well, Here we go again! For this screen you managed
 * to add a neat graph drawing using the instructions in the ssd1306 repo.
 *
 * Now you need to do a few things here to implement drawing the temp on the
 * graph: First implement the 'activate' function pointer for this screen that
 * clears out or mallocs nessesary data to do the averages, you could also
 * increase the 'refresh_ms' for this screen.
 *
 * Idea i had is as follows sample the temp every frame and add it to a total.
 * and then increment a counter to keep track of the amount of 'samples taken'
 *
 * after 2.5 minutes, calculate the average and draw a 3 pixel line on the grid
 * at the appropriate y location (x location is just time so just keep track of
 * a counter) empty the values and repeat.
 *
 * To make it look nice draw a line from the last pixel of the previous line to
 * the first pixel of the new line, this should make it look pretty nice i
 * think?
 */

void nf_calibration_screen_init(_nf_menu_t *_menu_state)
{
    _nf_menu_screen_t **_menu_screens_ptr = &_menu_state->menu_screens;

    _nf_menu_screen_t *calibration_screen =
        (_nf_menu_screen_t *)malloc(sizeof(_nf_menu_screen_t));
    calibration_screen->id = CALIBRATION_SCREEN_ID;
    calibration_screen->autoclear = false;
    calibration_screen->extra_data =
        malloc(sizeof(_nf_calibration_screen_state_t));

    _nf_calibration_screen_state_t *cali_state =
        (_nf_calibration_screen_state_t *)calibration_screen->extra_data;
    cali_state->_graph = (_nf_graph_state_t *)malloc(sizeof(_nf_graph_state_t));

    _nf_gen_calibration_screen_state(cali_state);

    _nf_menu_screen_fn_ptrs_t calibraion_screen_fns = {
        .on_render = nf_calibration_screen_render,
        .on_btn = nf_calibration_screen_btn_handler,
        .on_active = nf_calibration_screen_on_active};

    calibration_screen->fnptrs = calibraion_screen_fns;
    calibration_screen->next = NULL;

    nf_menu_add_screen(_menu_screens_ptr, calibration_screen);
}

void _nf_gen_calibration_screen_state(
    _nf_calibration_screen_state_t *_cali_state)
{
    _nf_graph_state_t *_graph = _cali_state->_graph;

    _graph->_sample_count = 0;
    _graph->_sum = 0.0;
    _graph->_avg_index = 0;
    _graph->_drawing = false;

    _graph->_temp_high_value = 50;
    _graph->_sec_high_value = 100;

    for (uint8_t i = 0; i < NF_GRAPH_NUM_SEGMENTS; i++)
    {
        _graph->_averages[i] = 0.0;
    }

    _graph->_sample_timeout = make_timeout_time_ms(NF_SAMPLE_TIMEOUT);
}

void nf_calibration_screen_render(_nf_menu_t *menu_state, ssd1306_t *disp_ptr,
                                  void *extra_data)
{
    ssd1306_clear(disp_ptr);

    _nf_calibration_screen_state_t *cali_state =
        (_nf_calibration_screen_state_t *)extra_data;
    _nf_graph_state_t *_graph = cali_state->_graph;
    nf_graph_render(menu_state, disp_ptr, _graph);
    return;
}

void nf_calibration_screen_btn_handler(_nf_menu_t *menu_state, uint btn,
                                       bool repeat, void *extra_data)
{
}

void nf_calibration_screen_on_active(_nf_menu_t *_menu_state, void *extra_data,
                                     void *on_active_extra_data)
{
    _nf_calibration_screen_state_t *cali_state =
        (_nf_calibration_screen_state_t *)extra_data;
    _nf_gen_calibration_screen_state(cali_state);
}
