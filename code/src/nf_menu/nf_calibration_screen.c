#include "nf_menu/nf_calibration_screen.h"
#include "nf_menu/nf_graph_font.h"

/*
 * TODO (DrNotThatEvil, 2024-01-15, 22:16):
 * Well well well, Here we go again! For this screen you managed
 * to add a neat graph drawing using the instructions in the ssd1306 repo.
 * 
 * Now you need to do a few things here to implement drawing the temp on the graph:
 * First implement the 'activate' function pointer for this screen that clears out or mallocs
 * nessesary data to do the averages, you could also increase the 'refresh_ms' for this screen.
 * 
 * Idea i had is as follows sample the temp every frame and add it to a total.
 * and then increment a counter to keep track of the amount of 'samples taken'
 * 
 * after 2.5 minutes, calculate the average and draw a 3 pixel line on the grid
 * at the appropriate y location (x location is just time so just keep track of a counter)
 * empty the values and repeat.
 * 
 * To make it look nice draw a line from the last pixel of the previous line to the
 * first pixel of the new line, this should make it look pretty nice i think?
*/

double my_ceil(double x) {
    int intPart = (int)x;
    if (x > intPart) {
        return intPart + 1.0;
    } else {
        return x;
    }
}

void nf_calibration_screen_init(_nf_menu_t* _menu_state)
{
    _nf_menu_screen_t** _menu_screens_ptr = &_menu_state->menu_screens;

    _nf_menu_screen_t* calibration_screen = (_nf_menu_screen_t*) malloc(sizeof(_nf_menu_screen_t));
    calibration_screen->id = CALIBRATION_SCREEN_ID;
    calibration_screen->autoclear = false;
    calibration_screen->extra_data = malloc(sizeof(_nf_calibration_screen_state_t));

    _nf_calibration_screen_state_t* cali_state = (_nf_calibration_screen_state_t*) calibration_screen->extra_data;
    _nf_gen_calibration_screen_state(cali_state);

    _nf_menu_screen_fn_ptrs_t calibraion_screen_fns = {
        .on_render = nf_calibration_screen_render,
        .on_btn = nf_calibration_screen_btn_handler,
        .on_active = nf_calibration_screen_on_active
    };

    calibration_screen->fnptrs = calibraion_screen_fns;
    calibration_screen->next = NULL;

    nf_menu_add_screen(_menu_screens_ptr, calibration_screen);
}

void _nf_gen_calibration_screen_state(_nf_calibration_screen_state_t* _cali_state)
{
    _cali_state->_sample_count = 0;
    _cali_state->_sum = 0.0;
    _cali_state->_avg_index = 0;
    _cali_state->_drawing = false;

    _cali_state->_temp_high_value = 50;
    _cali_state->_sec_high_value = 60;

    for(uint8_t i = 0; i < 9; i++)
    {
        _cali_state->_averages[i] = 0.0;
    }

    _cali_state->_sample_timeout = make_timeout_time_ms(NF_SAMPLE_TIMEOUT);
    _cali_state->_avg_timeout = make_timeout_time_ms(NF_AVG_TIMEOUT);
}

void _nf_format_gvalue(uint16_t value, char* target)
{
    sprintf(target, "%d", value);
    if (strlen(target) == 1)
    {
        sprintf(target, "::%d", value);
    }
    else if (strlen(target) == 2)
    {
        sprintf(target, ":%d", value);
    }

}

void nf_calibration_screen_render(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data)
{
    _nf_calibration_screen_state_t* cali_state = (_nf_calibration_screen_state_t*) extra_data;
    ssd1306_clear(disp_ptr);

    // Left Line
    ssd1306_draw_line(disp_ptr, 17, 0, 19, 0);
    ssd1306_draw_pixel(disp_ptr, 18, 25);
    ssd1306_draw_line(disp_ptr, 19, 0, 19, 50);
    
    // Bottom Line
    ssd1306_draw_line(disp_ptr, 17, 50, 119, 50);
    ssd1306_draw_line(disp_ptr, 69 , 50, 69, 52);
    ssd1306_draw_line(disp_ptr, 119 , 50, 119, 52);

    //ssd1306_draw_line(disp_ptr, 39 + (i * 20), 51, 39 + (i * 20), 52);

    char str[20];
    _nf_format_gvalue(cali_state->_sec_high_value / 2, str);
    ssd1306_draw_string_with_font(disp_ptr, 60, 55, 1, graph_font_5x5, str);
    
    _nf_format_gvalue(cali_state->_sec_high_value, str);
    ssd1306_draw_string_with_font(disp_ptr, 110, 55, 1, graph_font_5x5, str);

    ssd1306_draw_string_with_font(disp_ptr, 0, 50, 1, graph_font_5x5, "::0");
    
    _nf_format_gvalue(cali_state->_temp_high_value / 2, str);
    ssd1306_draw_string_with_font(disp_ptr, 0, 25, 1, graph_font_5x5, str);
    
    _nf_format_gvalue(cali_state->_temp_high_value, str);
    ssd1306_draw_string_with_font(disp_ptr, 0, 0, 1, graph_font_5x5, str);

    if(menu_state->heater_state == 1) {
        ssd1306_draw_square(disp_ptr, 120, 5, 5, 5);
    }

    uint32_t cy = 0;
    uint32_t ex = 0;

    if (cali_state->_drawing) {
        for(uint8_t i = 0; i < cali_state->_avg_index; i++)
        {
            uint32_t last_line_end_x = ex;
            uint32_t last_line_end_y = cy;
            
            double sx_d = (((double) i) * 5.0) / ((double) cali_state->_sec_high_value) * 100.0; 
            double ex_d = (((double) (i + 1)) * 5.0) / ((double) cali_state->_sec_high_value) * 100.0; 

            uint32_t sx = NF_GRAPH_ZERO_X + my_ceil(sx_d);
            ex = NF_GRAPH_ZERO_X + my_ceil(ex_d);
            //ex = ((((i + 1) * 5) / cali_state->_sec_high_value) * 100);
            cy = NF_GRAPH_ZERO_Y - ((NF_GRAPH_ZERO_Y + 1) * (cali_state->_averages[i] / cali_state->_temp_high_value));

            if(last_line_end_x > 0 && last_line_end_y > 0) 
            {
                ssd1306_draw_line(disp_ptr, 
                    last_line_end_x,
                    last_line_end_y,
                    sx,
                    cy
                );
            }

            ssd1306_draw_line(disp_ptr, 
                sx,
                cy,
                ex,
                cy
            );
        }
    }

    if(get_absolute_time()._private_us_since_boot < cali_state->_sample_timeout._private_us_since_boot)
    {
        return;
    }

    cali_state->_sample_timeout = make_timeout_time_ms(NF_SAMPLE_TIMEOUT);
    cali_state->_sum += menu_state->cur_temp;
    cali_state->_sample_count++;

    if(get_absolute_time()._private_us_since_boot > cali_state->_avg_timeout._private_us_since_boot)
    {
        double avg = cali_state->_sum / cali_state->_sample_count;
        cali_state->_averages[cali_state->_avg_index] = avg;
        cali_state->_drawing = true;

        if(cali_state->_sec_high_value < 500) {
            double sec_percentage = (((double)cali_state->_avg_index) * 5.0) / ((double) cali_state->_sec_high_value) ;
            if (sec_percentage > 0.7) {
                cali_state->_sec_high_value += 60;

                if(cali_state->_sec_high_value > 500) {
                    cali_state->_sec_high_value = 500;
                }
            }
        }

        if(cali_state->_temp_high_value < 200) {
            double temp_percentage = (avg / ((double) cali_state->_temp_high_value));
            if (temp_percentage > 0.7) {
                cali_state->_temp_high_value += 50;

                if(cali_state->_temp_high_value > 200) {
                    cali_state->_temp_high_value = 200;
                }
            }
        }

        /*
        uint32_t sx = NF_GRAPH_ZERO_X + cali_state->_avg_count;
        uint32_t ex = NF_GRAPH_ZERO_X + (cali_state->_avg_count + 1);
        uint32_t cy = NF_GRAPH_ZERO_Y - ((NF_GRAPH_ZERO_Y + 1) * (average / 200.0));

        if(cali_state->_last_line_end_x > 0 && cali_state->_last_line_end_y > 0) 
        {
            ssd1306_draw_line(disp_ptr, 
                cali_state->_last_line_end_x,
                cali_state->_last_line_end_y,
                sx,
                cy
            );
        }

        ssd1306_draw_line(disp_ptr, 
            sx,
            cy,
            ex,
            cy
        );


        ssd1306_clear_square(disp_ptr, 28, 4, 70, 10);
        char temp_str[20];
        sprintf(temp_str, "Avg: %.2f", average);
        ssd1306_draw_string(disp_ptr, 30, 5, 1, temp_str);
        */

        cali_state->_avg_index = (cali_state->_avg_index + 1) % 100;
        cali_state->_sum = 0.0;
        cali_state->_sample_count = 0;
        //cali_state->_last_line_end_x = ex;
        //cali_state->_last_line_end_y = cy; 
        cali_state->_avg_timeout = make_timeout_time_ms(NF_AVG_TIMEOUT);
    }

/*
    char temp_str[20];
    sprintf(temp_str, "Temp: %.2f", menu_state->cur_temp);
    ssd1306_draw_string(disp_ptr, 20, 5, 1, temp_str)

    uint32_t temp_line = NF_GRAPH_ZERO_Y - ((NF_GRAPH_ZERO_Y + 1) * (menu_state->cur_temp / 200.0));
    ssd1306_draw_line(disp_ptr, NF_GRAPH_ZERO_X, temp_line, NF_GRAPH_MAX_X, temp_line);
*/
}

void nf_calibration_screen_btn_handler(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data)
{

}

void nf_calibration_screen_on_active(_nf_menu_t* _menu_state, void* extra_data, void* on_active_extra_data)
{
    _nf_calibration_screen_state_t* cali_state = (_nf_calibration_screen_state_t*) extra_data;
    _nf_gen_calibration_screen_state(cali_state);
}

