#ifndef NF_TEMPSYS_H
#define NF_TEMPSYS_H

#include "nf_common.h"
#include "nf_max31855.h"

typedef enum {
    PRE_INIT,
    NORMAL,
    CALIBRATION
} _nf_tempsys_state_t;

typedef struct nf_temps {
    double internal;
    double thermocouple;
    double change_internal;
    double change_thermocouple;
} _nf_temps_t;

typedef struct nf_tempsys {
    _nf_max31855_t* _temp;
    _nf_tempsys_state_t _state;
    _nf_temps_t _results[2][2];
    uint read_index[2];
    uint write_index[2];
} _nf_tempsys_t;


void nf_initalize_tempsys(_nf_tempsys_t* _tempsys);
void nf_tempsys_loop(_nf_tempsys_t* _tempsys);

#endif