#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>

#include "nf_config.h"
#include "debounce.h"
//#include "font.h"
#include "ssd1306.h"

void setup_gpios(void);
void animation(void);
void gpio_callback(uint gpio, uint32_t events);

volatile uint counter;

int main()
{
    counter = 0;

    stdio_init_all();
    setup_gpios();
    animation();
}

void gpio_callback(uint gpio, uint32_t events)
{
    if(debounce(gpio)) {
        return;
    }

    if(gpio == 14) {
        counter++;
    }else if(gpio == 15 && counter > 0) {
        counter--;
    }
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

void animation(void)
{

    ssd1306_t disp;
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c1);
    ssd1306_clear(&disp);

    char str[10];

    for(;;)
    { 
        sprintf(str, "%d", counter);

        ssd1306_draw_string(&disp, 8, 24, 2, str);
        ssd1306_show(&disp);
        sleep_ms(25);
        ssd1306_clear(&disp);
    }
}