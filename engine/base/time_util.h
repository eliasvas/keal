#ifndef TIME_UTIL_H_
#define TIME_UTIL_H_

#include "base_inc.h"
#include <time.h>

static u64 get_current_timestamp() {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    u64 millis = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);

    return millis;
}


#endif