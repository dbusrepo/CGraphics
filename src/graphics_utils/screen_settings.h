#pragma once
#include <inttypes.h>
#include <stdbool.h>

typedef struct screen_settings {
    char *window_title;
    uint32_t xsize;
    uint32_t ysize;
    int targetFps;
    bool show_rendering_info;
} screen_settings_t;
