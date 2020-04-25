#pragma once
#include <stdbool.h>

typedef struct screen_x11 screen_x11_t;

typedef struct {
    char *window_title;
    int xsize;
    int ysize;
    int targetFps;
    bool show_rendering_info;
} screen_settings_t;

screen_x11_t *init_screen_x11(screen_settings_t *screen_settings);

typedef void (*update_fptr_t)(void);
typedef void (*render_fptr_t)(void);
typedef void (*key_event_fptr_t)(int key_code);

void run_event_loop(screen_x11_t *screen_x11,
                    update_fptr_t update_fptr,
                    render_fptr_t render_fptr,
                    key_event_fptr_t key_event_fptr);

