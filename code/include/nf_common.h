#ifndef NF_COMMON_H
#define NF_COMMON_H

#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

//// PIN CONFIG
// SCREEN
#define NF_SCREEN_SDA 14
#define NF_SCREEN_SCL 15

// INPUTS
#define NF_BTN0_PIN 10
#define NF_BTN1_PIN 11
#define NF_BTN2_PIN 12

// OUTPUTS
#define NF_SSR0_PIN 2
#define NF_SSR1_PIN 3

#define NF_FAN0_PIN 4
#define NF_FAN1_PIN 5

#define NF_SPKR_PIN 6

// SPI PINS
#define NF_SPI_SCK  18
#define NF_SPI_MISO 16
#define NF_TEMP0_CS 19
#define NF_TEMP1_CS 20

#define NF_MENU_BACK_BTN   NF_BTN0_PIN
#define NF_MENU_NEXT_BTN   NF_BTN1_PIN
#define NF_MENU_SELECT_BTN NF_BTN2_PIN
// #define NF_MENU_BACK_BTN NF_BTN3_PIN

#define NF_BACK_BTN_INDEX   0
#define NF_NEXT_BTN_INDEX   1
#define NF_SELECT_BTN_INDEX 2

// MEMORY RELATED
#define PROFILE_TARGETS_SIZE 4
#define NUMBER_OF_PROFILES   3

// THREAD MESSAGES
#define TEMP_CORE_STARTED_FLAG 123

// MESSAGE TYPES
// 0x01 to 0x0F TEMPSYS -> MENU
#define TEMPSYS_INITIALIZED_MSG_TYPE 0x01
#define TEMPSYS_ERROR_MSG_TYPE       0x02
#define TEMPSYS_TEMP_UPDATE_MSG_TYPE 0x03
#define TEMPSYS_FINISHED_MSG_TYPE    0x04

// TEMPSYS TEMP UPDATE MESSAGE MASKS
#define TEMPSYS_TEMP0_READABLE_MASK (1 << 0)
#define TEMPSYS_TEMP1_READABLE_MASK (1 << 1)
#define TEMPSYS_TEMP0_RESULT_MASK   (0b11 << 2)
#define TEMPSYS_TEMP1_RESULT_MASK   (0b11 << 4)

// TEMPSYS TEMP UPDATE MESSAGE MASK MACROS
#define SET_TEMPSYS_TEMP0_READABLE(flags) \
    ((flags) |= TEMPSYS_TEMP0_READABLE_MASK)
#define SET_TEMPSYS_TEMP1_READABLE(flags) \
    ((flags) |= TEMPSYS_TEMP1_READABLE_MASK)

#define SET_TEMPSYS_TEMP0_READABLE_INDEX(flags, index) \
    ((flags) = ((flags) & ~TEMPSYS_TEMP0_RESULT_MASK) | ((index) & 0b11 << 2))

#define SET_TEMPSYS_TEMP1_READABLE_INDEX(flags, index) \
    ((flags) = ((flags) & ~TEMPSYS_TEMP1_RESULT_MASK) | ((index) & 0b11 << 4))

#define GET_TEMPSYS_TEMP0_READABLE(flags) \
    ((flags) & TEMPSYS_TEMP0_READABLE_MASK)
#define GET_TEMPSYS_TEMP1_READABLE(flags) \
    ((flags) & TEMPSYS_TEMP1_READABLE_MASK)

#define GET_TEMPSYS_TEMP0_READABLE_INDEX(flags) \
    (((flags) & TEMPSYS_TEMP0_RESULT_MASK) >> 2)
#define GET_TEMPSYS_TEMP1_READABLE_INDEX(flags) \
    (((flags) & TEMPSYS_TEMP1_RESULT_MASK) >> 4)

// 0x10 to 0xF0 TEMPSYS -> MENU
#define MENU_STATE_CHANGE_MSG_TYPE 0x10
#define MENU_SET_PROFILE_MSG_TYPE  0x20

// TEMPSYS ERROR MESSAGE VALUES
#define TEMPSYS_GENRIC_ERROR                300
#define TEMPSYS_FAULT_ERROR                 301 // Not very specific my dude
#define TEMPSYS_TEMPRATURE_ERROR            302
#define TEMPSYS_TEMPRATURE_TO_LOW_ERROR     303
#define TEMPSYS_TEMPRATURE_HIGH_ERROR       304
#define TEMPSYS_TEMPRATURE_CHANGERATE_ERROR 305

// MENU MESSAGE VALUES
// #define MENU_STATE_CHANGE_NORMAL 1
// #define MENU_STATE_CHANGE_CALIBRATION 1

#endif
