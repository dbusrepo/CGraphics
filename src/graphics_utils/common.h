#pragma once
#define __USE_POSIX199309
#include <time.h>

#define BITS_PER_BYTE 8
#define MAX_BYTE 255

#define NANO_IN_MILLI 1000000
#define NANO_IN_SEC (1000 * NANO_IN_MILLI)

static inline int64_t nano_time(void) {
    struct timespec time;
//    https://stackoverflow.com/questions/3523442/difference-between-clock-realtime-and-clock-monotonic
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    return time.tv_sec * NANO_IN_SEC + time.tv_nsec;
}
