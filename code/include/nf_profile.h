#ifndef NF_PROFILE_H
#define NF_PROFILE_H

#include <pico/stdlib.h>
#include <pico/types.h>
#include "nf_common.h"
#include "nf_memory.h"

#define NPROFILES 3

typedef struct nf_profile
{
    uint targets[PROFILE_TARGETS_SIZE][2];
    bool initialized;
} _nf_profile_t;

void nf_profiles_init(_nf_profile_t* profiles_ptr, _nf_memory_state_t* _memory);

#endif