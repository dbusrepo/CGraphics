#pragma once

typedef struct screen_x11 screen_x11_t;

typedef struct {
    char *window_title;
    int xsize;
    int ysize;
} screen_settings_t;

screen_x11_t *init_screen_x11(screen_settings_t *screen_settings);
void run_event_loop(screen_x11_t *screen_x11,
                    void (*update)(void),
                    void (*frame)(void),
                    void (*key_handler)(int key_code),
                    void (*idle)(void));

