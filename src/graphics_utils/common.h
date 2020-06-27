#pragma once
//#define __USE_POSIX199309
#include <time.h>

#define BITS_PER_BYTE 8
#define MAX_BYTE 255
#define NANO_IN_MILLI 1000000l
#define NANO_IN_SEC (1000l * NANO_IN_MILLI)

void flush_printf(const char *format, ...);
