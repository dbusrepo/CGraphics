#pragma once

#include "../graphics_utils/screen_settings.h"
#include "../graphics_utils/screen_callback.h"
#include "../graphics_utils/input_keys.h"

typedef void (*fun_update_t)(int64_t elapsed_time);
typedef void (*fun_render_t)(void);

typedef struct app app_t;

app_t *init_app(screen_settings_t *screen_settings);

void run_app(app_t *app,
             fun_update_t update_fun,
             fun_render_t render_fun,
             fun_key_t key_fun);

void toggle_fullscreen_app(app_t *app);
void terminate_app(app_t *app); // used in the call back

///* Image/texture information */
//typedef struct {
//    int Width, Height;
//    int Format;
//    int BytesPerPixel;
//    unsigned char *Data;
//} GLFWimage;
