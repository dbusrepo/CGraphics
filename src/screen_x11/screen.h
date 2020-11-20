#pragma once
#include <stdbool.h>
#include "../graphics_utils/screen_settings.h"
#include "../graphics_utils/screen_info.h"
#include "../graphics_utils/screen_callbacks.h"
#include "../graphics_utils/input_keys.h"

typedef struct screen screen_t;

screen_t* screen_init(screen_settings_t *screen_settings);

screen_t* screen_finish(screen_t **screen);

screen_info_t* screen_get_info(screen_t *screen);

void screen_poll_events(screen_t *screen);

void screen_blit(screen_t *screen);

void screen_terminate(screen_t *screen);

void screen_toggle_fullscreen(screen_t *screen);

void screen_set_key_callback(screen_t *screen, fun_key_t key_callback);

void screen_set_char_callback(screen_t *screen, fun_char_t char_callback);

void screen_set_mouse_pos_callback(screen_t *screen,
		fun_mouse_pos_t mouse_pos_callback);

void screen_set_mouse_button_callback(screen_t *screen,
		fun_mouse_button_t mouse_button_callback);

void screen_set_mouse_wheel_callback(screen_t *screen,
		fun_mouse_wheel_t mouse_wheel_callback);

void screen_win_size_callback(screen_t *screen,
		fun_win_size_t win_size_callback);

void screen_set_win_close_callback(screen_t *screen,
		fun_win_close_t win_close_callback);

void screen_set_win_refresh_callback(screen_t *screen,
		fun_win_refresh_t win_refresh_callback);

int screen_get_key(screen_t *screen, int key);

int screen_get_mouse(screen_t *screen, int button);

void screen_get_mouse_position(screen_t *screen, int *x, int *y);

void screen_set_mouse_position(screen_t *screen, int x, int y);

int screen_get_mouse_wheel(screen_t *screen);

void screen_set_mouse_wheel(screen_t *screen, int pos);
