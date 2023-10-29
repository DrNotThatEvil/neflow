#ifndef debounce_h
#define debounce_h

#include <pico/stdlib.h>

#define NGPIO 5
#define DEBOUNCE_MS 500

bool debounce(uint gpio);

#endif