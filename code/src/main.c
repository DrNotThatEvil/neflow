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
#include "nf_tempsys.h"
#include "pwm-tone.h"
#include "pitches.h"

#include "debounce.h"
#include "ssd1306.h"

#define BTN_UPDATE_TIMEOUT_MS 50
#define BTN_REPEAT_UNTIL_HELD 8
#define WIDTH 128
#define HEIGHT 64

static nf_state_t* _state;
//static absolute_time_t btn_update_timeout;

void setup_gpios(void);
void update(nf_state_t* _state);
void gpio_callback(uint gpio, uint32_t events);
void ui_core_entry();
void get_temp();

int main()
{
    stdio_init_all();
    setup_gpios();
    
    //get_temp();

    //btn_update_timeout = make_timeout_time_ms(BTN_UPDATE_TIMEOUT_MS);
    _state = malloc(sizeof(nf_state_t));

    nf_init(_state);
    multicore_launch_core1(ui_core_entry);

    multicore_fifo_push_blocking(TEMP_CORE_STARTED_FLAG);
    while (1) {
        nf_tempsys_loop(_state);
    }
    //update(_state);
}

void ui_core_entry(void) 
{
    for(;;)
    {
        menu_update(_state->_menu);
        _nf_temps_t* temp0_temps = (_nf_temps_t*) &(_state->_tempsys->_results[0][_state->_tempsys->read_index[0]]);

        nf_menu_update_cur_temp(_state->_menu, temp0_temps->thermocouple);
        
        uint heater_state = _state->_tempsys->heater_state;
        nf_menu_update_heater_state(_state->_menu, heater_state);
    }
}

void gpio_callback(uint gpio, uint32_t events)
{
    menu_gpio_callback(_state->_menu, gpio, events);
}


/*
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
*/

/*
 * TODO (DrNotThatEvil, 2024-01-10, 19:27):
 * The menu system has been reworked a little bit, i'm kind of thinking of keeping the nf_config.h
 * instead of moving it to the other name cause it's a big rework for just a rename and most systems
 * do actually depend on eachtorher now. 
 * 
 * the screen might be moved from nf_menu.c/nf_menu.h because that's not uses anywhere else.
 * but since the dog really wants attention i will let you decide that later.
 * 
*/

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

/*
void update(nf_state_t* _state)
{
    bool error_triggered = false;
    uint error_type = 0;
    uint error_animation = 0;
    uint32_t temp_started = multicore_fifo_pop_blocking();
    if(temp_started != TEMP_CORE_STARTED_FLAG) {
        error_triggered = true;
    }

    ssd1306_clear(_state->_disp_ptr);
    for(;;)
    {
        if(error_triggered) {
            ssd1306_clear(_state->_disp_ptr);
            if(error_animation == 0) {
                char str[20];
                sprintf(str, "ERROR: %d!!", error_type);
                ssd1306_draw_string(_state->_disp_ptr, 5, 10, 2, str);
                ssd1306_draw_string(_state->_disp_ptr, 5, 35, 1, "Reset oven!");
            }
            ssd1306_show(_state->_disp_ptr);
            sleep_ms(1000);
            error_animation = (error_animation + 1) % 2;
            continue;
        } else {

            if (multicore_fifo_rvalid()) {
                uint32_t message = multicore_fifo_pop_blocking();
                
                if(message >= TEMP_ERROR_FLAG) {
                    error_triggered = true;
                    error_type = message;
                    continue;
                }
            }

            _nf_temps_t* temp0_temps = (_nf_temps_t*) &(_state->_tempsys->_results[0][_state->_tempsys->read_index[0]]);
            nf_menu_update_cur_temp(_state->_menu, temp0_temps->thermocouple);
            update_btns();

            ssd1306_clear(_state->_disp_ptr);

            nf_menu_render(_state->_menu);
            ssd1306_show(_state->_disp_ptr);

            sleep_ms(_state->_menu->refresh_ms);
        }
    }
}
*/