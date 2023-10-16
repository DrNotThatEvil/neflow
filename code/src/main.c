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

    _state = malloc(sizeof(nf_state_t));
    nf_init(_state);

    setup_gpios();
    animation(_state);
}

void gpio_callback(uint gpio, uint32_t events)
{
    if(debounce(gpio)) {
        return;
    }

    if (gpio == 14) {
        if (_state->_menu->current_menu_option > 0) {
            _state->_menu->current_menu_option = ((_state->_menu->current_menu_option - 1) % NOPTIONS);
        } else {
            _state->_menu->current_menu_option = (NOPTIONS - 1);
        }
        
        return;
    }

    _state->_menu->current_menu_option = ((_state->_menu->current_menu_option + 1) % NOPTIONS);
}

void setup_gpios(void)
{
    i2c_init(i2c1, 400000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);
    
    gpio_set_irq_enabled_with_callback(NF_MENU_UP_BTN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled(NF_MENU_DOWN_BTN, GPIO_IRQ_EDGE_RISE, true);
}

void animation(nf_state_t* _state)
{

    ssd1306_t disp;
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c1);
    ssd1306_clear(&disp);

    char str[10];

    for(;;)
    {
        sprintf(str, "%s", _state->_menu->menu_options[_state->_menu->current_menu_option]);

        //ssd1306_draw_line(&disp, 49, 10, 64, 0);
        //ssd1306_draw_line(&disp, 64, 0, 79, 10);
        //ssd1306_draw_string(&disp, 24, 25, 2, str);

        for(int i = 0; i < (60/5); i++) {
            ssd1306_draw_line(&disp, 0, (5 * i), 5, (5 * i));
        }

        ssd1306_draw_line(&disp, 1, 0, 1, 60);
        ssd1306_draw_line(&disp, 2, 0, 2, 60);
        ssd1306_draw_line(&disp, 5, 60, 128, 60);
        ssd1306_draw_line(&disp, 5, 61, 128, 61);

        int prevX = 0, prevY = HEIGHT;
        int x, y;

        for (x = 0; x < WIDTH; x++) {
            // Simulate a hill-like shape (you can modify this function for different hill shapes)
            y = HEIGHT - (int)(50 * sin(0.06 * x) + x * x * 0.0002);

            ssd1306_draw_line(&disp, prevX, prevY, x, y);

            prevX = x;
            prevY = y;


            sleep_ms(100);
        }

        ssd1306_show(&disp);
        sleep_ms(200);
        ssd1306_clear(&disp);
    }
}