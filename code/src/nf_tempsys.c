#include "nf_tempsys.h"

void nf_tempsys_init(_nf_tempsys_t* _tempsys)
{
    _tempsys->_temp  = malloc(sizeof(_nf_max31855_t));
    nf_max31855_init(_tempsys->_temp);

    _tempsys->_state = PRE_INIT;
    _tempsys->_tempmode = UNKNOWN;

    for(uint i = 0; i < 2; i++) {
        for(uint j = 0; j < 2; j++) {
            _nf_temps_t empty_temp = { .internal = 0.0, .thermocouple = 0.0, .change_internal = 0.0, .change_thermocouple = 0.0 };
            _tempsys->_results[i][j] = empty_temp;
        }
    }

    _tempsys->read_index[0] = 0;
    _tempsys->read_index[1] = 0;
    
    _tempsys->write_index[0] = 1;
    _tempsys->write_index[1] = 1;
}

void nf_tempsys_loop(_nf_tempsys_t* _tempsys)
{
    // Initaization takes 2 loops, one to determine tempmode and read frist temp.
    // Second one does the same but also gets correct change from previous reading.
    // else first reading would be a verry high change rate.
    if (_tempsys->_state == PRE_INIT || _tempsys->_state == POST_INIT) {
        // Determine temprature mode
        _nf_max31855_result_t temp0_result;
        nf_max31855_read(_tempsys->_temp, NF_TEMP0_CS, &temp0_result);
        _nf_tempsys_update_temps(_tempsys, &temp0_result);

        _nf_max31855_result_t temp1_result;
        nf_max31855_read(_tempsys->_temp, NF_TEMP1_CS, &temp1_result);
        _nf_tempsys_update_temps(_tempsys, &temp1_result);

        if(temp0_result.faults == 0 && temp1_result.faults == 0) {
            _tempsys->_tempmode = USE_BOTH;
        } else if (temp0_result.faults == 0 && temp1_result.faults > 0) {
            _tempsys->_tempmode = USE_TEMP0;
        } else if (temp0_result.faults > 0 && temp1_result.faults == 0) {
            _tempsys->_tempmode = USE_TEMP1;
        } else {
            _tempsys->_state = ERROR;
            return;
        }

        _nf_swap_indexes(_tempsys);

        if (_tempsys->_state == PRE_INIT) {
            _tempsys->_state = POST_INIT;
            return;
        } else {
            _tempsys->_state = NORMAL;
            return;
        }
    }

    if(_tempsys->_state == ERROR) {
        _nf_trigger_error(_tempsys);
        return;
    }

    _nf_max31855_result_t temp0_result;
    _nf_max31855_result_t temp1_result;

    if(_tempsys->_tempmode == USE_TEMP0 || _tempsys->_tempmode == USE_BOTH) {
        nf_max31855_read(_tempsys->_temp, NF_TEMP0_CS, &temp0_result);
        _nf_tempsys_update_temps(_tempsys, &temp0_result);

        if(temp0_result.faults > 0) {
            // We where using this sensor fine before this VERRY WEIRD and bad
            _tempsys->_state = ERROR;
            _nf_trigger_error(_tempsys);
            return;
        }
    }
    
    if(_tempsys->_tempmode == USE_TEMP1 || _tempsys->_tempmode == USE_BOTH) {
        nf_max31855_read(_tempsys->_temp, NF_TEMP1_CS, &temp1_result);
        _nf_tempsys_update_temps(_tempsys, &temp1_result);

        if(temp1_result.faults > 0) {
            // We where using this sensor fine before this VERRY WEIRD and bad
            _tempsys->_state = ERROR;
            _nf_trigger_error(_tempsys);
            return;
        }
    }

    _nf_swap_indexes(_tempsys);

    // SANITY CHECK (normal just for now)
    if(_tempsys->_tempmode == USE_TEMP0) {
        _nf_temps_t* temps = (_nf_temps_t*) &(_tempsys->_results[0][_tempsys->read_index[0]]);
        
        if (temps->change_thermocouple > NORMAL_ALLOWED_CHANGERATE) {
            // SOMETHING BAD!
            _tempsys->_state = ERROR;
            _nf_trigger_error(_tempsys);
        }
    }
}

void _nf_tempsys_update_temps(_nf_tempsys_t* _tempsys, _nf_max31855_result_t* results)
{
    // Pins are in order so this is a neat trick.
    uint index = (results->sensor - NF_TEMP0_CS);
    if(results->faults == 0) {
        _nf_temps_t* temps = (_nf_temps_t*) &(_tempsys->_results[index][_tempsys->write_index[index]]);
        temps->internal = results->internal;
        temps->thermocouple = results->thermocouple;
        temps->change_internal = 0.0;
        temps->change_thermocouple = 0.0;

        if(_tempsys->_state != PRE_INIT) {
            _nf_temps_t* prev_temps = (_nf_temps_t*) &(_tempsys->_results[index][_tempsys->read_index[index]]);
            temps->change_internal = (temps->internal - prev_temps->internal);
            temps->change_thermocouple = (temps->thermocouple - prev_temps->thermocouple);
        }
    }
}

void _nf_swap_indexes(_nf_tempsys_t* _tempsys)
{
    uint tmp_windex_0 = _tempsys->write_index[0];
    uint tmp_windex_1 = _tempsys->write_index[1];
    _tempsys->write_index[0] = _tempsys->read_index[0];
    _tempsys->write_index[1] = _tempsys->read_index[1];
    
    _tempsys->read_index[0] = tmp_windex_0;
    _tempsys->read_index[1] = tmp_windex_1;
}

void _nf_trigger_error(_nf_tempsys_t* _tempsys)
{
    multicore_fifo_push_blocking(TEMP_ERROR_FLAG);
}