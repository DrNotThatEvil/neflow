#include "nf_menu/nf_menu_common.h"

void draw_edit_item(ssd1306_t *disp_ptr, uint y, bool selected, const char *str)
{
    ssd1306_draw_string(disp_ptr, 5, (y + 2), 1, str);

    if (selected)
    {
        ssd1306_draw_line(disp_ptr, 1, y, 127, y);
        ssd1306_draw_line(disp_ptr, 1, (y + 11), 127, (y + 11));
    }
}

void draw_edit_value(ssd1306_t *disp_ptr, uint y, bool selected,
                     const char *str)
{
    ssd1306_draw_string(disp_ptr, 55, (y + 4), 1, str);

    if (selected)
    {
        ssd1306_draw_line(disp_ptr, 44, (y + 6), 44, (y + 7));
        ssd1306_draw_line(disp_ptr, 45, (y + 5), 45, (y + 8));
        ssd1306_draw_line(disp_ptr, 46, (y + 4), 46, (y + 9));
        ssd1306_draw_line(disp_ptr, 47, (y + 3), 47, (y + 10));
        ssd1306_draw_line(disp_ptr, 48, (y + 2), 48, (y + 11));

        ssd1306_draw_line(disp_ptr, 84, (y + 2), 84, (y + 11));
        ssd1306_draw_line(disp_ptr, 85, (y + 3), 85, (y + 10));
        ssd1306_draw_line(disp_ptr, 86, (y + 4), 86, (y + 9));
        ssd1306_draw_line(disp_ptr, 87, (y + 5), 87, (y + 8));
        ssd1306_draw_line(disp_ptr, 88, (y + 6), 88, (y + 7));
        // ssd1306_draw_line(disp_ptr, 1, (y+11), 127, (y+11));
    }
}

void nf_menu_add_screen(_nf_menu_screen_t **_menu_screens_ptr,
                        _nf_menu_screen_t *_new_screen)
{
    // If the list is empty, make the new node the head of the list
    if (*_menu_screens_ptr == NULL)
    {
        *_menu_screens_ptr = _new_screen;
    }
    else
    {
        // Find the last node and update its 'next' pointer
        _nf_menu_screen_t *current = *_menu_screens_ptr;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = _new_screen;
    }
}

void nf_menu_change_screen_with_data(_nf_menu_t *_menu, uint screen_id,
                                     void *on_active_extra_data)
{
    _nf_menu_screen_t **current = &(_menu->menu_screens);

    while (*current != NULL)
    {
        if ((*current)->id == screen_id)
        {
            _menu->current_screen = current;
            if ((*_menu->current_screen)->fnptrs.on_active != NULL)
            {
                (*_menu->current_screen)
                    ->fnptrs.on_active(_menu,
                                       (*_menu->current_screen)->extra_data,
                                       on_active_extra_data);
            }

            return;
        }
        current = &((*current)->next);
    }
}

void nf_menu_change_screen(_nf_menu_t *_menu, uint screen_id)
{
    nf_menu_change_screen_with_data(_menu, screen_id, NULL);
}

void nf_menu_change_state(_nf_menu_t *_menu, _nf_menu_state_t state)
{
    // NOTE (DrNotThatEvil, 2024-12-05): the FINISHED state gets set by
    // tempsys so this should never, send a message about that.
    _menu->_state = state;
    _nf_thread_msg menu_state_change_msg = {.msg_type =
                                                MENU_STATE_CHANGE_MSG_TYPE,
                                            .simple_msg_value = ((uint)state)};

    queue_add_blocking(&_menu->tempsys_msg_q, &menu_state_change_msg);
}

void nf_set_profile(_nf_menu_t *_menu, void *profile)
{
    _nf_thread_msg profile_msg = {.msg_type = MENU_SET_PROFILE_MSG_TYPE,
                                  .value_ptr = profile};

    queue_add_blocking(&_menu->tempsys_msg_q, &profile_msg);
}

void draw_prev_section(ssd1306_t *disp_ptr, const char *str)
{
    ssd1306_draw_pixel(disp_ptr, 2, 3);
    ssd1306_draw_line(disp_ptr, 3, 2, 3, 4);
    ssd1306_draw_line(disp_ptr, 4, 1, 4, 5);

    ssd1306_draw_string(disp_ptr, 8, 0, 1, str);

    ssd1306_draw_line(disp_ptr, 0, 9, 128, 9);
}

void draw_next_arrow(ssd1306_t *disp_ptr)
{
    ssd1306_draw_line(disp_ptr, 122, 28, 122, 35);
    ssd1306_draw_line(disp_ptr, 123, 29, 123, 34);
    ssd1306_draw_line(disp_ptr, 124, 30, 124, 33);
    ssd1306_draw_line(disp_ptr, 125, 31, 125, 32);
}

void _nf_format_gvalue(uint16_t value, char *target)
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

double my_ceil(double x)
{
    int intPart = (int)x;
    if (x > intPart)
    {
        return intPart + 1.0;
    }
    else
    {
        return x;
    }
}

void nf_graph_render(_nf_menu_t *_menu_state, ssd1306_t *disp_ptr,
                     _nf_graph_state_t *_graph)
{
    // Left Line
    ssd1306_draw_line(disp_ptr, 17, 0, 19, 0);
    ssd1306_draw_pixel(disp_ptr, 18, 25);
    ssd1306_draw_line(disp_ptr, 19, 0, 19, 50);

    // Bottom Line
    ssd1306_draw_line(disp_ptr, 17, 50, 119, 50);
    ssd1306_draw_line(disp_ptr, 69, 50, 69, 52);
    ssd1306_draw_line(disp_ptr, 119, 50, 119, 52);

    // ssd1306_draw_line(disp_ptr, 39 + (i * 20), 51, 39 + (i * 20), 52);

    char str[20];
    _nf_format_gvalue(_graph->_sec_high_value / 2, str);
    ssd1306_draw_string_with_font(disp_ptr, 60, 55, 1, graph_font_5x5, str);

    _nf_format_gvalue(_graph->_sec_high_value, str);
    ssd1306_draw_string_with_font(disp_ptr, 110, 55, 1, graph_font_5x5, str);

    ssd1306_draw_string_with_font(disp_ptr, 0, 50, 1, graph_font_5x5, "::0");

    _nf_format_gvalue(_graph->_temp_high_value / 2, str);
    ssd1306_draw_string_with_font(disp_ptr, 0, 25, 1, graph_font_5x5, str);

    _nf_format_gvalue(_graph->_temp_high_value, str);
    ssd1306_draw_string_with_font(disp_ptr, 0, 0, 1, graph_font_5x5, str);

    if (_menu_state->heater_state == 1)
    {
        ssd1306_draw_square(disp_ptr, 120, 5, 5, 5);
    }

    uint32_t cy = 0;
    uint32_t ex = 0;

    double l_length = 100.0 / (NF_GRAPH_NUM_SEGMENTS - 1.0);
    if (_graph->_drawing)
    {
        for (uint8_t i = 0; i < _graph->_avg_index; i++)
        {
            uint32_t last_line_end_x = ex;
            uint32_t last_line_end_y = cy;

            double sx_d = (((double)i) * l_length);
            double ex_d = (((double)(i + 1)) * l_length);

            uint32_t sx = NF_GRAPH_ZERO_X + my_ceil(sx_d);
            ex = NF_GRAPH_ZERO_X + my_ceil(ex_d);
            // ex = ((((i + 1) * 5) / cali_state->_sec_high_value) * 100);
            cy = NF_GRAPH_ZERO_Y -
                 ((NF_GRAPH_ZERO_Y + 1) *
                  (_graph->_averages[i] / _graph->_temp_high_value));

            if (last_line_end_x > 0 && last_line_end_y > 0)
            {
                ssd1306_draw_line(disp_ptr, last_line_end_x, last_line_end_y,
                                  sx, cy);
            }

            ssd1306_draw_line(disp_ptr, sx, cy, ex, cy);
        }

        /*
        double l_length = (100.0 / ((double) NF_GRAPH_NUM_SEGMENTS));

        for(uint8_t i = 0; i < _graph->_avg_index; i++)
        {
            uint32_t last_line_end_x = ex;
            uint32_t last_line_end_y = cy;

            double sx_d = (((double) i) * l_length);
            double ex_d = (((double) (i + 1)) * l_length);

            uint32_t sx = NF_GRAPH_ZERO_X + my_ceil(sx_d);
            ex = NF_GRAPH_ZERO_X + my_ceil(ex_d);
            //ex = ((((i + 1) * 5) / cali_state->_sec_high_value) * 100);
            cy = NF_GRAPH_ZERO_Y - ((NF_GRAPH_ZERO_Y + 1) *
        (_graph->_averages[i] / _graph->_temp_high_value));

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
        */
    }

    if (get_absolute_time() < _graph->_sample_timeout)
    {
        return;
    }

    // TODO (DrNotThatEvil, 2024-06-09): This needs to increase if the array has
    // been condensed.
    _graph->_sample_timeout = make_timeout_time_ms(NF_SAMPLE_TIMEOUT);
    _graph->_sum += _menu_state->cur_temp;
    _graph->_sample_count++;

    // long timeout = (((double)NF_GRAPH_NUM_SEGMENTS) / NF_SAMPLE_TIMEOUT) *
    // 100.0;
    uint16_t tavg = _graph->_sec_high_value / NF_GRAPH_NUM_SEGMENTS;
    float sample_target_sec = (float)NF_GRAPH_SAMPLE_PERIOD / 1000.0;
    uint16_t sample_target = (uint16_t)(tavg / sample_target_sec);

    if (_graph->_sample_count > sample_target)
    {
        double avg = _graph->_sum / _graph->_sample_count;
        if (_graph->_avg_index < NF_GRAPH_NUM_SEGMENTS)
        {
            _graph->_averages[_graph->_avg_index] = avg;
        }
        else
        {
            float temp[(NF_GRAPH_NUM_SEGMENTS / 2)];
            for (uint8_t i = 0; i < (NF_GRAPH_NUM_SEGMENTS / 2); i++)
            {
                temp[i] =
                    (_graph->_averages[2 * i] + _graph->_averages[2 * i + 1]) /
                    2.0f;
            }

            for (uint8_t i = 0; i < (NF_GRAPH_NUM_SEGMENTS / 2); i++)
            {
                _graph->_averages[i] = temp[i];
            }

            _graph->_sec_high_value =
                (_graph->_sec_high_value * NF_GRAPH_SCALE_FACTOR);
            _graph->_averages[(NF_GRAPH_NUM_SEGMENTS / 2)] = avg;
            _graph->_avg_index = (NF_GRAPH_NUM_SEGMENTS / 2);
        }

        _graph->_drawing = true;

        if (_graph->_temp_high_value < 200)
        {
            double temp_percentage = (avg / ((double)_graph->_temp_high_value));
            if (temp_percentage > 0.7)
            {
                _graph->_temp_high_value += 50;

                if (_graph->_temp_high_value > 200)
                {
                    _graph->_temp_high_value = 200;
                }
            }
        }

        _graph->_avg_index = (_graph->_avg_index + 1) % 100;
        _graph->_sum = 0.0;
        _graph->_sample_count = 0;
    }
}
