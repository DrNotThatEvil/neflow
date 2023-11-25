#include "nf_memory.h"

void nf_memory_init(_nf_memory_state_t* _memory)
{
    _memory->was_saved = false;
    _memory->empty_page_index = -1;
    nf_memory_get_first_empty_page(_memory);

    // If the the last empty page is 0 it means no previous value was stored.
    // If higher then it there was a saved value.
    _memory->was_saved = (_memory->empty_page_index > 0);

}

void nf_memory_get_first_empty_page(_nf_memory_state_t* _memory)
{
    int addr;
    int *p;

    for(uint page = 0; page < FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE; page++)
    {
        addr = XIP_BASE + FLASH_TARGET_OFFSET + (page * FLASH_PAGE_SIZE);
        p = (int *) addr;

        if( *p == -1 && _memory->empty_page_index < 0) {
            _memory->empty_page_index = page;
        }
    }
}