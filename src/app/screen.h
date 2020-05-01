#pragma once
#include <stdbool.h>

#include "../graphics_utils/screen_settings.h"
#include "../graphics_utils/screen_info_rgb.h"
#include "../graphics_utils/screen_callback.h"
#include "../graphics_utils/input_keys.h"

typedef struct screen screen_t;

screen_t *init_screen(screen_settings_t *screen_settings);
screen_info_rgb_t *get_screen_info(screen_t *screen);
void poll_events(screen_t *screen);
void blit(screen_t *screen);
void terminate_screen(screen_t *screen);

void set_key_callback(screen_t *screen, fun_key_t key_fun);
