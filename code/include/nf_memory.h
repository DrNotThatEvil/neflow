#ifndef NF_MEMORY_H
#define NF_MEMORY_H

#include <hardware/sync.h>
#include <hardware/flash.h>
#include "nf_common.h"

#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)
#define PID_KP_INDEX 0
#define PID_KI_INDEX 1
#define PID_KD_INDEX 2

typedef struct nf_memory {
    float pid_data[3];
    uint profile_data[NUMBER_OF_PROFILES][PROFILE_TARGETS_SIZE][2];
} _nf_memory_t;

typedef struct nf_memory_state {
    bool was_saved;
    int empty_page_index;
    _nf_memory_t current_buffer[FLASH_PAGE_SIZE/sizeof(_nf_memory_t)];
} _nf_memory_state_t;

void nf_memory_init(_nf_memory_state_t* _memory);
void nf_memory_get_first_empty_page(_nf_memory_state_t* _memory);
void nf_memory_save(_nf_memory_state_t* _memory);


#endif