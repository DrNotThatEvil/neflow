#ifndef NF_PROFILE_H
#define NF_PROFILE_H

#include <pico/stdlib.h>
#include <pico/types.h>
#include "nf_common.h"
#include "nf_memory.h"

#define NPROFILES 3

typedef struct nf_profile
{
    uint targets[PROFILE_TARGETS_SIZE][2]; // 0 -> temp, 1 - target time.
    bool initialized;
} _nf_profile_t;

typedef struct nf_profile_state
{
    _nf_memory_state_t* _memory;
    _nf_profile_t* _profiles;
} _nf_profile_state_t;

void nf_profiles_init(_nf_profile_state_t* _profile_state, _nf_memory_state_t* _memory);
void nf_profile_save(_nf_profile_state_t* _profile_state, _nf_profile_t* _profile);

#endif