#include "nf_memory.h"

void nf_memory_init(_nf_memory_state_t* _memory)
{
    _memory->was_saved = false;
    _memory->empty_page_index = -1;
    nf_memory_get_first_empty_page(_memory);

    // If the the last empty page is 0 it means no previous value was stored.
    // If higher then it there was a saved value.
    _memory->was_saved = (_memory->empty_page_index > 0);

    if(!_memory->was_saved) {
        if(!_memory->empty_page_index == -1) {
            nf_memory_load_last_page(_memory);
            nf_memory_clear(_memory);
            nf_memory_save(_memory);
            return;
        }

        _memory->current_buffer->pid_data[0] = 1.0f;
        _memory->current_buffer->pid_data[1] = 1.0f;
        _memory->current_buffer->pid_data[2] = 1.0f;

        for(uint i = 0; i < NUMBER_OF_PROFILES; i++) 
        {
            for(uint j = 0; j < PROFILE_TARGETS_SIZE; j++) 
            {
                _memory->current_buffer->profile_data[i][j][0] = 0.f;
                _memory->current_buffer->profile_data[i][j][1] = 0.f;
            }
        } 
    }

    if(_memory->was_saved) {
        nf_memory_load_page(_memory, (_memory->empty_page_index - 1));
    }
}

void nf_memory_get_first_empty_page(_nf_memory_state_t* _memory)
{
    int addr;
    int *p;

    for(uint page = 0; page < FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE; page++)
    {
        addr = XIP_BASE + FLASH_TARGET_OFFSET + (page * FLASH_PAGE_SIZE);
        p = (int*) addr;

        if( *p == -1 && _memory->empty_page_index < 0) {
            _memory->empty_page_index = page;
        }
    }
}

void nf_memory_clear(_nf_memory_state_t* _memory)
{
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    _memory->empty_page_index = 0;
    restore_interrupts(ints);
}

void nf_memory_load_page(_nf_memory_state_t* _memory, uint page)
{
    int addr = XIP_BASE + FLASH_TARGET_OFFSET + (page * FLASH_PAGE_SIZE);
    _nf_memory_state_t *p = (_nf_memory_state_t*) addr;
        
    _memory->current_buffer->pid_data[0] = p->current_buffer->pid_data[0];
    _memory->current_buffer->pid_data[1] = p->current_buffer->pid_data[1];
    _memory->current_buffer->pid_data[2] = p->current_buffer->pid_data[2];

    for(uint i = 0; i < NUMBER_OF_PROFILES; i++) 
    {
        for(uint j = 0; j < PROFILE_TARGETS_SIZE; j++) 
        {
            _memory->current_buffer->profile_data[i][j][0] = p->current_buffer->profile_data[i][j][0];
            _memory->current_buffer->profile_data[i][j][1] = p->current_buffer->profile_data[i][j][1];
        }
    }
}

void nf_memory_load_last_page(_nf_memory_state_t* _memory) 
{
    uint max_page = ((FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE) - 1);
    nf_memory_load_page(_memory, max_page);
}

void nf_memory_save(_nf_memory_state_t* _memory)
{
    uint32_t ints = save_and_disable_interrupts();
    flash_range_program(FLASH_TARGET_OFFSET + (_memory->empty_page_index*FLASH_PAGE_SIZE), (uint8_t *)_memory->current_buffer, FLASH_PAGE_SIZE);
    restore_interrupts(ints);

    /*
        TODO:
        You need to fix this function to increment the 'empty_page_index',
        currently every time you save it's written at the same index. 

        Also if the page you are going te write to is above last page you need to write to clear the memory
        and write to the last first page. 
    */
}