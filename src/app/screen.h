#pragma once
#include <stdbool.h>

#include "../graphics_utils/screen_settings.h"
#include "../graphics_utils/screen_info_rgb.h"
#include "../graphics_utils/screen_callback.h"
#include "../graphics_utils/input_keys.h"

typedef struct screen screen_t;

screen_t *init_screen(screen_settings_t *screen_settings);
screen_info_rgb_t *get_screen_info(screen_t *screen);
void poll_events_screen(screen_t *screen);
void blit_screen(screen_t *screen);
void terminate_screen(screen_t *screen);
void toggle_fullscreen_mode(screen_t *screen);

void set_key_callback_screen(screen_t *screen, fun_key_t key_callback);
void set_char_callback_screen(screen_t *screen, fun_char_t char_callback);
void set_mouse_pos_callback_screen(screen_t *screen, fun_mouse_pos_t mouse_pos_callback);
void set_mouse_button_callback_screen(screen_t *screen, fun_mouse_button_t mouse_button_callback);
void set_mouse_wheel_callback_screen(screen_t *screen, fun_mouse_wheel_t mouse_wheel_callback);

void set_win_size_callback_screen(screen_t *screen, fun_win_size_t win_size_callback);
void set_win_close_callback_screen(screen_t *screen, fun_win_close_t win_close_callback);
void set_win_refresh_callback_screen(screen_t *screen, fun_win_refresh_t win_refresh_callback);
