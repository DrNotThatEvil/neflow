#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <pico/types.h>
#include <pico/multicore.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>

#include "nf_common.h"
#include "nf_config.h"
#include "pwm-tone.h"
#include "pitches.h"

#include "debounce.h"
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
void ui_core_entry();

int main()
{
    stdio_init_all();
    setup_gpios();

    btn_update_timeout = make_timeout_time_ms(BTN_UPDATE_TIMEOUT_MS);
    _state = malloc(sizeof(nf_state_t));

    nf_init(_state);
    multicore_launch_core1(ui_core_entry);

    while (1) 
        tight_loop_contents();
    //update(_state);
}

void ui_core_entry(void) 
{
    update(_state);
}


void gpio_callback(uint gpio, uint32_t events)
{
    if(events & GPIO_IRQ_EDGE_FALL) {
        if(debounce(gpio)) {
            return;
        }

        if (gpio == NF_MENU_BACK_BTN && !_state->_buttons[NF_BACK_BTN_INDEX].pressed) {
            _state->_buttons[NF_BACK_BTN_INDEX].pressed = true;
            nf_menu_btn_handler(_state->_menu, 0, false);

            return;
            //_state->_menu->menu_options[_state->_menu->current_menu_option].back_fn((void*) _state->_menu, false);
        }

        if (gpio == NF_MENU_NEXT_BTN && !_state->_buttons[NF_NEXT_BTN_INDEX].pressed) {
            _state->_buttons[NF_NEXT_BTN_INDEX].pressed = true;
            nf_menu_btn_handler(_state->_menu, 1, false);

            return;
            //_state->_menu->menu_options[_state->_menu->current_menu_option].next_fn((void*) _state->_menu, false);
        }

        if (gpio == NF_MENU_SELECT_BTN && !_state->_buttons[NF_SELECT_BTN_INDEX].pressed) {
            _state->_buttons[NF_SELECT_BTN_INDEX].pressed = true;
            nf_menu_btn_handler(_state->_menu, 2, false);

            return;
            //_state->_menu->menu_options[_state->_menu->current_menu_option].select_fn((void*) _state->_menu, false);
        }
    }

    if(events & GPIO_IRQ_EDGE_RISE) {
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
                    nf_menu_btn_handler(_state->_menu, 0, true);
                    //_state->_menu->menu_options[_state->_menu->current_menu_option].back_fn((void*) _state->_menu, true);
                }

                if(i == NF_NEXT_BTN_INDEX) {
                    nf_menu_btn_handler(_state->_menu, 1, true);
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
    // Screen setup
    i2c_init(i2c1, 400000);
    gpio_set_function(NF_SCREEN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(NF_SCREEN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(NF_SCREEN_SDA);
    gpio_pull_up(NF_SCREEN_SCL);

    // Speaker setup
    //tone_init(&generator, NF_SPKR_PIN);

    // OUTPUTS
    gpio_init(NF_SSR0_PIN);
    gpio_set_dir(NF_SSR0_PIN, GPIO_OUT);

    gpio_init(NF_SSR1_PIN);
    gpio_set_dir(NF_SSR1_PIN, GPIO_OUT);
    
    gpio_init(NF_FAN0_PIN);
    gpio_set_dir(NF_FAN0_PIN, GPIO_OUT);
    
    gpio_init(NF_FAN1_PIN);
    gpio_set_dir(NF_FAN1_PIN, GPIO_OUT);

    // BUTTONS
    gpio_init(NF_MENU_BACK_BTN);
    gpio_set_dir(NF_MENU_BACK_BTN, GPIO_IN);
    
    gpio_init(NF_MENU_NEXT_BTN);
    gpio_set_dir(NF_MENU_NEXT_BTN, GPIO_IN);
    
    gpio_init(NF_MENU_SELECT_BTN);
    gpio_set_dir(NF_MENU_SELECT_BTN, GPIO_IN);

    gpio_set_irq_enabled_with_callback(NF_MENU_BACK_BTN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(NF_MENU_SELECT_BTN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(NF_MENU_NEXT_BTN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}

void update(nf_state_t* _state)
{

    ssd1306_clear(_state->_disp_ptr);

    for(;;)
    {
        update_btns();

        ssd1306_clear(_state->_disp_ptr);
        nf_menu_render(_state->_menu);
        ssd1306_show(_state->_disp_ptr);

        sleep_ms(_state->_menu->refresh_ms);
    }
}