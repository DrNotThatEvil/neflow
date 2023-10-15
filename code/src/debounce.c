#include "debounce.h"

static volatile absolute_time_t dbounce_timeouts[NGPIO] = { 0u };

bool debounce(uint gpio)
{
    if (get_absolute_time()._private_us_since_boot < dbounce_timeouts[(gpio % NGPIO)]._private_us_since_boot) {
        return true;
    }

    dbounce_timeouts[(gpio % NGPIO)] = make_timeout_time_ms(DEBOUNCE_MS);
    return false;
}