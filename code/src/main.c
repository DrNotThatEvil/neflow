#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <pico/types.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>

#include "nf_config.h"
#include "debounce.h"
//#include "font.h"
#include "ssd1306.h"

#define BTN_UPDATE_TIMEOUT_MS 50
#define BTN_REPEAT_UNTIL_HELD 8
#define WIDTH 128
#define HEIGHT 64

static nf_state_t* _state;
static absolute_time_t btn_update_timeout = { 0u };

void setup_gpios(void);
void update(nf_state_t* _state);
void gpio_callback(uint gpio, uint32_t events);


int main()
{
    stdio_init_all();
    setup_gpios();

    btn_update_timeout = make_timeout_time_ms(BTN_UPDATE_TIMEOUT_MS);
    _state = malloc(sizeof(nf_state_t));
    nf_init(_state);
    update(_state);
}

void gpio_callback(uint gpio, uint32_t events)
{
    if((events & GPIO_IRQ_EDGE_RISE) != 0) {
        if(debounce(gpio)) {
            return;
        }

        if (gpio == NF_MENU_BACK_BTN && !_state->_buttons[NF_BACK_BTN_INDEX].pressed) {
            _state->_buttons[NF_BACK_BTN_INDEX].pressed = true;
            nf_menu_btn_handler(_state->_menu, 0, false);
            //_state->_menu->menu_options[_state->_menu->current_menu_option].back_fn((void*) _state->_menu, false);
        }

        if (gpio == NF_MENU_NEXT_BTN && !_state->_buttons[NF_NEXT_BTN_INDEX].pressed) {
            _state->_buttons[NF_NEXT_BTN_INDEX].pressed = true;
            //_state->_menu->menu_options[_state->_menu->current_menu_option].next_fn((void*) _state->_menu, false);
        }

        if (gpio == NF_MENU_SELECT_BTN && !_state->_buttons[NF_SELECT_BTN_INDEX].pressed) {
            _state->_buttons[NF_SELECT_BTN_INDEX].pressed = true;
            //_state->_menu->menu_options[_state->_menu->current_menu_option].select_fn((void*) _state->_menu, false);
        }

        return;
    }

    if (gpio == NF_MENU_BACK_BTN) {
        _state->_buttons[NF_BACK_BTN_INDEX].released = true;
        //_state->_buttons[NF_BACK_BTN_INDEX].held = false;
        //_state->_buttons[NF_BACK_BTN_INDEX].pressed = false;
    }

    if (gpio == NF_MENU_NEXT_BTN) {
        _state->_buttons[NF_NEXT_BTN_INDEX].released = true;
        //_state->_buttons[NF_NEXT_BTN_INDEX].held = false;
        //_state->_buttons[NF_NEXT_BTN_INDEX].pressed = false;
    }

    if (gpio == NF_MENU_SELECT_BTN) {
        _state->_buttons[NF_SELECT_BTN_INDEX].released = true;
        //_state->_buttons[NF_SELECT_BTN_INDEX].held = false;
        //_state->_buttons[NF_SELECT_BTN_INDEX].pressed = false;
    }
}

void update_btns(void)
{
    if (get_absolute_time()._private_us_since_boot < btn_update_timeout._private_us_since_boot) {
        return;
    }
    
    btn_update_timeout = make_timeout_time_ms(BTN_UPDATE_TIMEOUT_MS);

    for(int i=0; i < 3; i++) {
        if(_state->_buttons[i].pressed && !_state->_buttons[i].held && !_state->_buttons[i].released) {
            _state->_buttons[i].held = true;
        }

        if(_state->_buttons[i].pressed && _state->_buttons[i].held && !_state->_buttons[i].released) {
            // Button being held repeat event here.
            // only back and next should have repeat events
            if(_state->_buttons[i].held_count > BTN_REPEAT_UNTIL_HELD) {
                if(i == NF_BACK_BTN_INDEX) {
                    //_state->_menu->menu_options[_state->_menu->current_menu_option].back_fn((void*) _state->_menu, true);
                }

                if(i == NF_NEXT_BTN_INDEX) {
                    //_state->_menu->menu_options[_state->_menu->current_menu_option].next_fn((void*) _state->_menu, true);
                }
            } else {
                _state->_buttons[i].held_count = (_state->_buttons[i].held_count + 1);
            }
            continue;
        }

        
        if(_state->_buttons[i].released && _state->_buttons[i].pressed) {
            
            // Fire released event here.. if i ever had one... (yes i'm picuteing the farily odd parents meme atm...)
            _state->_buttons[i].released = false;
            _state->_buttons[i].held = false;
            _state->_buttons[i].held_count = 0;
            _state->_buttons[i].pressed = false;
        }
    }

}

void setup_gpios(void)
{
    i2c_init(i2c1, 400000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);
    
    gpio_set_irq_enabled_with_callback(NF_MENU_BACK_BTN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(NF_MENU_SELECT_BTN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(NF_MENU_NEXT_BTN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}

void update(nf_state_t* _state)
{

    ssd1306_clear(_state->_disp_ptr);

    for(;;)
    {

        update_btns();
        /*
        sprintf(str, "%s", _state->_menu->menu_options[_state->_menu->current_menu_option]);

        ssd1306_draw_line(_state->_disp_ptr, 49, 10, 64, 0);
        ssd1306_draw_line(_state->_disp_ptr, 64, 0, 79, 10);

        ssd1306_draw_string(_state->_disp_ptr, 20, 22, 2, str);

        ssd1306_draw_line(_state->_disp_ptr, 49, 50, 64, 60);
        ssd1306_draw_line(_state->_disp_ptr, 64, 60, 79, 50);
        */

        ssd1306_clear(_state->_disp_ptr);
        nf_menu_render(_state->_menu);
        ssd1306_show(_state->_disp_ptr);

        sleep_ms(_state->_menu->refresh_ms);
    }
}