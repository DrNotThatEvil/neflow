#include "nf_tempsys.h"

void nf_tempsys_init(_nf_tempsys_t* _tempsys)
{
    _tempsys->_temp  = malloc(sizeof(_nf_max31855_t));
    nf_max31855_init(_tempsys->_temp);

    _tempsys->_state = PRE_INIT;
    for(uint i = 0; i < 2; i++) {
        for(uint i = 0; i < 2; i++) {
        }
    }

    _tempsys->read_index[0] = 0;
    _tempsys->read_index[1] = 0;
    
    _tempsys->write_index[0] = 1;
    _tempsys->write_index[1] = 1;
}

void nf_tempsys_loop(_nf_tempsys_t* _tempsys)
{
    // Read temps.

}
