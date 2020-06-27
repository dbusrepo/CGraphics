#include <stdio.h>
#include <stdarg.h>
#include "common.h"

void flush_printf(const char *format, ...)
{
    va_list arg;
    va_start (arg, format);
    vprintf(format, arg);
    va_end (arg);
    fflush(stdout);
}
