#include "nf_profile.h"

void nf_profiles_init(_nf_profile_state_t* _profile_state, _nf_memory_state_t* _memory)
{

    _profile_state->_memory = _memory;
    _profile_state->_profiles = (_nf_profile_t*) malloc(NUMBER_OF_PROFILES * sizeof(_nf_profile_t));

    for(uint i = 0; i < NUMBER_OF_PROFILES; i++) 
    {
        for(uint j = 0; j < PROFILE_TARGETS_SIZE; j++) 
        {
            if(_memory->current_buffer->profile_data[i][j][0] == 0.0f || 
                _memory->current_buffer->profile_data[i][j][1] == 0.0f) {
                _profile_state->_profiles[i].targets[j][0] = 1;
                _profile_state->_profiles[i].targets[j][1] = 1;
                _profile_state->_profiles[i].initialized = false;
                continue;
            }

            _profile_state->_profiles[i].targets[j][0] = _memory->current_buffer->profile_data[i][j][0]; 
            _profile_state->_profiles[i].targets[j][1] = _memory->current_buffer->profile_data[i][j][1]; 
            _profile_state->_profiles[i].initialized = true;
        }
    }
}

void nf_profiles_save(_nf_profile_state_t* _profile_state, _nf_profile_t* _profile)
{
    ptrdiff_t offset = _profile - _profile_state->_profiles;
    for(uint j = 0; j < PROFILE_TARGETS_SIZE; j++) 
    {
        _profile_state->_memory->current_buffer->profile_data[offset][j][0] = _profile->targets[j][0];
        _profile_state->_memory->current_buffer->profile_data[offset][j][1] = _profile->targets[j][1];
    }

    nf_memory_save(_profile_state->_memory);
}