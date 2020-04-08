#pragma once

typedef struct graph_app graph_app_t;

typedef struct screen_settings {
    unsigned char *screen_buffer;
    unsigned long red_mask, green_mask, blue_mask;
    int bytes_per_pixel; // 4
    int bytes_per_rgb; // 3
} screen_settings_t;

void init_screen_settings(graph_app_t *, screen_settings_t *);
void draw_event(graph_app_t *);
void update_event(graph_app_t *);
void key_event(graph_app_t *, int ch);