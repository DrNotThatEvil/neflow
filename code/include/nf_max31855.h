#ifndef NF_MAX31855_H
#define NF_MAX31855_H

#include "nf_common.h"
#include <pico/stdlib.h>
#include <pico/binary_info.h>
#include <hardware/spi.h>

#define MAX31855_FAULT     0x100    // 1 when any of the SCV, SCG, or OC faults are active
#define MAX31855_SCV_FAULT  0x04    // short-circuited to VCC
#define MAX31855_SCG_FAULT  0x02    // short-circuited to GND
#define MAX31855_OC_FAULT   0x01    // open (no connections)

#define MAX31855_SIGN_EXTEND(value, bit) (((value) ^ (1u << ((bit) - 1))) - (1u << ((bit) - 1)))

typedef struct nf_max31855 {
    spi_inst_t *spi;
} _nf_max31855_t;

typedef struct nf_max31855_result {
    uint sensor;
    bool status;
    double thermocouple;
    double internal;
    uint8_t faults;
} _nf_max31855_result_t;

void nf_max31855_init(_nf_max31855_t* sensor_state);
void nf_max31855_read(_nf_max31855_t* sensor_state, uint temp_sensor, _nf_max31855_result_t* result);

#endif