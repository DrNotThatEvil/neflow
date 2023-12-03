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

            profiles_ptr[i].initialized = false;
        }

        return;
    }

    for(uint i = 0; i < NUMBER_OF_PROFILES; i++) 
    {
        for(uint j = 0; j < PROFILE_TARGETS_SIZE; j++) 
        {
            if(_memory->current_buffer->profile_data[i][j][0] == 0.0f || 
                _memory->current_buffer->profile_data[i][j][1] == 0.0f) {
                profiles_ptr[i].targets[j][0] = 1;
                profiles_ptr[i].targets[j][1] = 1;
                profiles_ptr[i].initialized = false;
                continue;
            }

            profiles_ptr[i].targets[j][0] = _memory->current_buffer->profile_data[i][j][0]; 
            profiles_ptr[i].targets[j][1] = _memory->current_buffer->profile_data[i][j][1]; 
            profiles_ptr[i].initialized = true;
        }

    }
}