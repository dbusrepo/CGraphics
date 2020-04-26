#pragma once
#include <stdbool.h>

//#include "../graphics_utils/screen_info_rgb.h"
typedef struct screen_info_rgb screen_info_rgb_t;
typedef struct screen_x11 screen_x11_t;

typedef struct {
    char *window_title;
    int xsize;
    int ysize;
    int targetFps;
    bool show_rendering_info;
} screen_settings_t;

typedef void (*update_fptr_t)(void);
typedef void (*render_fptr_t)(void);
typedef void (*key_event_fptr_t)(int key_code);

screen_x11_t *init_screen_x11(screen_settings_t *screen_settings);

void run_event_loop(screen_x11_t *screen_x11,
                    update_fptr_t update_fptr,
                    render_fptr_t render_fptr,
                    key_event_fptr_t key_event_fptr);

screen_info_rgb_t *get_screen_info(screen_x11_t *screen_x11);
