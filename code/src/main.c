#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>

#include "nf_config.h"
#include "debounce.h"
//#include "font.h"
#include "ssd1306.h"

#define WIDTH 128
#define HEIGHT 64

static nf_state_t* _state;

void setup_gpios(void);
void animation(nf_state_t* _state);
void gpio_callback(uint gpio, uint32_t events);


int main()
{
    stdio_init_all();
    setup_gpios();

    _state = malloc(sizeof(nf_state_t));
    nf_init(_state);
    animation(_state);
}

void gpio_callback(uint gpio, uint32_t events)
{
    if(debounce(gpio)) {
        return;
    }

    if (gpio == NF_MENU_BACK_BTN) {
        _state->_menu->menu_options[_state->_menu->current_menu_option].back_fn((void*) _state->_menu);
    }

    if (gpio == NF_MENU_NEXT_BTN) {
        _state->_menu->menu_options[_state->_menu->current_menu_option].next_fn((void*) _state->_menu);
    }

    if (gpio == NF_MENU_SELECT_BTN) {
        _state->_menu->menu_options[_state->_menu->current_menu_option].select_fn((void*) _state->_menu);
    }
}

void setup_gpios(void)
{
    i2c_init(i2c1, 400000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);
    
    gpio_set_irq_enabled_with_callback(NF_MENU_BACK_BTN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled(NF_MENU_SELECT_BTN, GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(NF_MENU_NEXT_BTN, GPIO_IRQ_EDGE_RISE, true);
}

void animation(nf_state_t* _state)
{

    ssd1306_clear(_state->_disp_ptr);

    for(;;)
    {
        /*
        sprintf(str, "%s", _state->_menu->menu_options[_state->_menu->current_menu_option]);

        ssd1306_draw_line(_state->_disp_ptr, 49, 10, 64, 0);
        ssd1306_draw_line(_state->_disp_ptr, 64, 0, 79, 10);

        ssd1306_draw_string(_state->_disp_ptr, 20, 22, 2, str);

        ssd1306_draw_line(_state->_disp_ptr, 49, 50, 64, 60);
        ssd1306_draw_line(_state->_disp_ptr, 64, 60, 79, 50);
        */

        nf_menu_render(_state->_menu);

        ssd1306_show(_state->_disp_ptr);
        sleep_ms(20);
        ssd1306_clear(_state->_disp_ptr);
    }
}