#ifndef TIME_UTIL_H
#define TIME_UTIL_H

#include "base_inc.h"
#include <time.h>

static u64 get_current_timestamp() {
    u64 millis = 0;
#if (OS_WINDOWS)
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    // Convert the FILETIME structure to a 64-bit integer
    uint64_t time = (((uint64_t)ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
    // Convert the time from 100-nanosecond intervals to milliseconds
    millis = (time - 116444736000000000ULL) / 10000;
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    millis = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
#endif

    return millis;
}

static f64 get_current_timestamp_sec() {
    return (get_current_timestamp()/1000.0);
}

#endif