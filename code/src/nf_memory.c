#include "nf_memory.h"

void nf_memory_init(_nf_memory_state_t *_memory)
{
    // nf_memory_clear(_memory);

    _memory->current_buffer = malloc(sizeof(_nf_memory_t));
    nf_initalize_empty(_memory);

    _memory->was_saved = false;
    _memory->empty_page_index = -1;
    nf_memory_get_first_empty_page(_memory);

    // If the the last empty page is 0 it means no previous value was stored.
    // If higher then it there was a saved value.
    _memory->was_saved = (_memory->empty_page_index > 0);

    if (!_memory->was_saved && _memory->empty_page_index == -1)
    {
        if (nf_memory_load_last_page(_memory))
        {
            return;
        }

        // load failed
    }

    if (_memory->was_saved)
    {
        nf_memory_load_page(_memory, (_memory->empty_page_index - 1));

        // load failed
    }
}

void nf_memory_get_first_empty_page(_nf_memory_state_t *_memory)
{
    int addr;
    int *p;

    for (uint page = 0; page < FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE; page++)
    {
        addr = XIP_BASE + FLASH_TARGET_OFFSET + (page * FLASH_PAGE_SIZE);
        p = (int *)addr;

        if (*p == -1 && _memory->empty_page_index < 0)
        {
            _memory->empty_page_index = page;
        }
    }
}

void nf_initalize_empty(_nf_memory_state_t *_memory)
{
    _memory->current_buffer->pid_data[0][0] = 1.0f;
    _memory->current_buffer->pid_data[0][1] = 1.0f;
    _memory->current_buffer->pid_data[0][2] = 1.0f;

    _memory->current_buffer->pid_data[1][0] = 1.0f;
    _memory->current_buffer->pid_data[1][1] = 1.0f;
    _memory->current_buffer->pid_data[1][2] = 1.0f;

    for (uint i = 0; i < NUMBER_OF_PROFILES; i++)
    {
        for (uint j = 0; j < PROFILE_TARGETS_SIZE; j++)
        {
            _memory->current_buffer->profile_data[i][j][0] = 0.f;
            _memory->current_buffer->profile_data[i][j][1] = 0.f;
        }
    }
}

void nf_memory_clear(_nf_memory_state_t *_memory)
{
    uint32_t ints = save_and_disable_interrupts();
    multicore_lockout_start_blocking(); // Stop core1
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    multicore_lockout_end_blocking(); // Restart core1
    restore_interrupts(ints);
    _memory->empty_page_index = 0;
}

bool nf_memory_load_page(_nf_memory_state_t *_memory, uint page)
{
    uint32_t ints = save_and_disable_interrupts();
    int addr = XIP_BASE + FLASH_TARGET_OFFSET + (page * FLASH_PAGE_SIZE);

    uint8_t *buffer = (uint8_t *)addr;
    if (*buffer != NF_MEMORY_PREFIX_BYTE)
    {
        return false;
    }

    _nf_memory_t *p_memory = (_nf_memory_t *)(buffer + sizeof(uint8_t));

    memcpy(&_memory->current_buffer->pid_data[0][0], &p_memory->pid_data[0][0],
           sizeof(float));
    memcpy(&_memory->current_buffer->pid_data[0][1], &p_memory->pid_data[0][1],
           sizeof(float));
    memcpy(&_memory->current_buffer->pid_data[0][2], &p_memory->pid_data[0][2],
           sizeof(float));
    memcpy(&_memory->current_buffer->pid_data[1][0], &p_memory->pid_data[1][0],
           sizeof(float));
    memcpy(&_memory->current_buffer->pid_data[1][1], &p_memory->pid_data[1][1],
           sizeof(float));
    memcpy(&_memory->current_buffer->pid_data[1][2], &p_memory->pid_data[1][2],
           sizeof(float));

    for (uint i = 0; i < NUMBER_OF_PROFILES; i++)
    {
        for (uint j = 0; j < PROFILE_TARGETS_SIZE; j++)
        {
            memcpy(&_memory->current_buffer->profile_data[i][j][0],
                   &p_memory->profile_data[i][j][0], sizeof(uint));
            memcpy(&_memory->current_buffer->profile_data[i][j][1],
                   &p_memory->profile_data[i][j][1], sizeof(uint));
        }
    }

    restore_interrupts(ints);
}

bool nf_memory_load_last_page(_nf_memory_state_t *_memory)
{
    uint max_page = ((FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE) - 1);
    return nf_memory_load_page(_memory, max_page);
}

void nf_memory_save(_nf_memory_state_t *_memory)
{
    uint max_page = ((FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE) - 1);
    // Check if on max or no empty page available (same as reaching max page)
    if (_memory->empty_page_index < max_page && _memory->empty_page_index != -1)
    {
        uint32_t ints = save_and_disable_interrupts();
        multicore_lockout_start_blocking(); // Stop core1

        uint8_t *data_to_write =
            (uint8_t *)malloc(sizeof(uint8_t) + sizeof(_nf_memory_t));
        data_to_write[0] = (uint8_t)NF_MEMORY_PREFIX_BYTE;
        memcpy(&data_to_write[1], _memory->current_buffer,
               sizeof(_nf_memory_t));
        flash_range_program(FLASH_TARGET_OFFSET +
                                (_memory->empty_page_index * FLASH_PAGE_SIZE),
                            data_to_write, FLASH_PAGE_SIZE);

        multicore_lockout_end_blocking(); // Restart core1
        restore_interrupts(ints);

        _memory->empty_page_index = _memory->empty_page_index + 1;
        free(data_to_write);
        return;
    }
    else
    {
        nf_memory_clear(_memory);
        nf_memory_save(_memory);
    }
}

void nf_memory_save_with_callback(_nf_memory_state_t *_memory,
                                  MemorySaveCallback cb, void *ptr)
{
    nf_memory_save(_memory);
    cb(ptr);
}
