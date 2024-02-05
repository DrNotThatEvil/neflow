#ifndef NF_COMMON_H
#define NF_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <pico/stdlib.h>

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
#define NF_SPI_SCK 18
#define NF_SPI_MISO 16
#define NF_TEMP0_CS 19
#define NF_TEMP1_CS 20

#define NF_MENU_BACK_BTN NF_BTN0_PIN
#define NF_MENU_NEXT_BTN NF_BTN1_PIN
#define NF_MENU_SELECT_BTN NF_BTN2_PIN
//#define NF_MENU_BACK_BTN NF_BTN3_PIN

#define NF_BACK_BTN_INDEX 0
#define NF_NEXT_BTN_INDEX 1
#define NF_SELECT_BTN_INDEX 2

// MEMORY RELATED
#define PROFILE_TARGETS_SIZE 4
#define NUMBER_OF_PROFILES 3

// THREAD MESSAGES 
#define TEMP_CORE_STARTED_FLAG 123

#define TEMPSYS_MSG_ERROR_TYPE 1
#define TEMPSYS_MSG_TEMP_UPDATE_TYPE 2

#define TEMPSYS_GENRIC_ERROR 300
#define TEMPSYS_FAULT_ERROR 301 // Not very specific my dude
#define TEMPSYS_TEMPRATURE_ERROR 302

#define STATE_CHANGE_MSG 0x01
#define STATE_CHANGE_MSG_VALUE_CALIBRATION 0x01

#endif