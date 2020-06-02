#pragma once

typedef void (*fun_key_t)(int key, int action);

typedef void (*fun_char_t)(char character, int action);

typedef void (*fun_mouse_pos_t)(int x, int y);

typedef void (*fun_mouse_button_t)(int button, int action);

typedef void (*fun_mouse_wheel_t)(int pos);

typedef int (*fun_win_close_t)(void); // ritorna 1 bool che indica se terminare
typedef void (*fun_win_size_t)(int width, int height);

typedef int (*fun_win_refresh_t)(void);
