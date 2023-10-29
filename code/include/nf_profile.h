#ifndef NF_PROFILE_H
#define NF_PROFILE_H

#include <pico/stdlib.h>
#include <pico/types.h>

#define NPROFILES 3

typedef struct {
    uint pre_heat_start_temp;
    uint pre_heat_start_target;

    uint pre_heat_end_temp;
    uint pre_heat_end_target;
    
    uint ramp_up_temp;
    uint ramp_up_target;

    uint peak_temp;
    uint peak_target;
    bool initialized;
} nf_profile_t;

void nf_profiles_init(nf_profile_t* profiles_ptr);

#endif