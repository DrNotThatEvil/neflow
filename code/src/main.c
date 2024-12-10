#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <math.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <pico/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "nf_common.h"

#include "nf_config.h"
#include "nf_tempsys.h"
#include "pitches.h"
#include "pwm-tone.h"

#include "debounce.h"
#include "ssd1306.h"

#define BTN_UPDATE_TIMEOUT_MS 50
#define BTN_REPEAT_UNTIL_HELD 8
#define WIDTH                 128
#define HEIGHT                64

static nf_state_t *_state;
// static absolute_time_t btn_update_timeout;

void setup_gpios(void);
void update(nf_state_t *_state);
void gpio_callback(uint gpio, uint32_t events);
void ui_core_entry();
void get_temp();

int main()
{
    stdio_init_all();
    setup_gpios();

    _state = malloc(sizeof(nf_state_t));

    nf_init(_state);
    multicore_launch_core1(ui_core_entry);

    while (1)
    {
        nf_tempsys_loop(_state);
    }
}

void ui_core_entry(void)
{
    multicore_lockout_victim_init();
    for (;;)
    {
        menu_update(_state->_menu);

        uint heater_state = _state->_tempsys->heater_state;
        nf_menu_update_heater_state(_state->_menu, heater_state);
    }
}

void gpio_callback(uint gpio, uint32_t events)
{
    menu_gpio_callback(_state->_menu, gpio, events);
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
    // tone_init(&generator, NF_SPKR_PIN);

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

    gpio_set_irq_enabled_with_callback(NF_MENU_BACK_BTN,
                                       GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                       true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(NF_MENU_SELECT_BTN,
                                       GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                       true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(NF_MENU_NEXT_BTN,
                                       GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                       true, &gpio_callback);
}
