#include "nf_max31855.h"

static inline void nf_max31855_cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(NF_TEMP0_CS, 0);
    asm volatile("nop \n nop \n nop");
}

static inline void nf_max31855_cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(NF_TEMP0_CS, 1);
    asm volatile("nop \n nop \n nop");
}

void nf_max31855_init(_nf_max31855_t* sensor_state)
{
    sensor_state->spi = spi0;

    gpio_set_function(NF_SPI_MISO, GPIO_FUNC_SPI);
    gpio_set_function(NF_SPI_SCK, GPIO_FUNC_SPI);

    bi_decl(bi_2pins_with_func(NF_SPI_MISO, NF_SPI_SCK, GPIO_FUNC_SPI));

    gpio_init(NF_TEMP0_CS);
    gpio_set_dir(NF_TEMP0_CS, GPIO_OUT);
    gpio_put(NF_TEMP0_CS, 1);

    bi_decl(bi_1pin_with_name(NF_TEMP0_CS, "SPI CS"));

    spi_init(sensor_state->spi, 500000);
    nf_max31855_cs_deselect();

    //gpio_init(NF_TEMP1_CS);
    //gpio_set_dir(NF_TEMP1_CS, GPIO_OUT);
    //gpio_put(NF_TEMP1_CS, 1);
}

void nf_max31855_read(_nf_max31855_t* sensor_state, uint temp_sensor, _nf_max31855_result_t* result)
{
    result->sensor = 0;
    result->status = false;
    result->thermocouple = 0.0;
    result->internal = 0.0;
    result->faults = 0;

    uint32_t data = 0;
    uint8_t buf[4];

    nf_max31855_cs_select();
    spi_read_blocking(sensor_state->spi, 0, buf, 4);
    nf_max31855_cs_deselect();

    data = buf[0];
    data <<= 8;
    data |= buf[1];
    data <<= 8;
    data |= buf[2];
    data <<= 8;
    data |= buf[3];

    result->faults = (data & 0x7); // TODO convert to constant.

    if (data & 0x80000000) {
        // Negative value, drop the lower 18 bits and explicitly extend sign bits.
        result->thermocouple = (double) (0xFFFFC000 | ((data >> 18) & 0x00003FFF));
    } else {
        // Positive value, just drop the lower 18 bits.
        result->thermocouple = (double) (data >> 18);
    }
    result->thermocouple *= 0.25;

    int32_t tmp = (data >> 4);
    result->internal = (double) (tmp & 0x7FF);
    if(tmp & 0x800) {
        result->internal = (double) (0xF800 | (tmp & 0x7FF));
    }
    result->internal *= 0.0625;

}