#pragma once
#include <stdbool.h>

#include "../graphics_utils/screen_settings.h"
#include "../graphics_utils/screen_info_rgb.h"

typedef struct app app_t;
typedef void (*app_key_event_fptr_t)(app_t *app, int key_code);

typedef struct screen screen_t;

screen_t *init_screen(screen_settings_t *screen_settings);
screen_info_rgb_t *get_screen_info(screen_t *screen);
void check_event(screen_t *screen, app_t *app, app_key_event_fptr_t app_key_event_fptr);
void blit(screen_t *screen);
void close_screen(screen_t *screen);
