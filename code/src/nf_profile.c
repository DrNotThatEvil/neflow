#include "nf_profile.h"

void nf_profiles_init(_nf_profile_t* profiles_ptr, _nf_memory_state_t* _memory)
{

    if (!_memory->was_saved) {
        for(uint i = 0; i < NUMBER_OF_PROFILES; i++) 
        {

            for(uint j = 0; j < PROFILE_TARGETS_SIZE; j++) 
            {
                profiles_ptr[i].targets[j][0] = 1;
                profiles_ptr[i].targets[j][1] = 1;
            }

            /*
            profiles_ptr[i].pre_heat_start_temp = 1;
            profiles_ptr[i].pre_heat_start_target = 1;
            
            profiles_ptr[i].pre_heat_end_temp = 1;
            profiles_ptr[i].pre_heat_end_target = 1;
            
            profiles_ptr[i].ramp_up_temp = 1;
            profiles_ptr[i].ramp_up_target = 1;
            
            profiles_ptr[i].peak_temp = 1;
            profiles_ptr[i].peak_target = 1;
            */

            profiles_ptr[i].initialized = false;
        }
    }
}