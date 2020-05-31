#pragma once

#include "../graphics_utils/screen_settings.h"
#include "../graphics_utils/screen_callbacks.h"
#include "../graphics_utils/input_keys.h"

typedef void (*fun_update_t)(int64_t elapsed_time);
typedef void (*fun_render_t)(void);

typedef struct app app_t;

app_t *app_init(screen_settings_t *screen_settings);

void app_run(app_t *app,
             fun_update_t update_fun,
             fun_render_t render_fun,
             fun_key_t key_fun);

void app_toggle_fullscreen(app_t *app);
void app_terminate(app_t *app); // used in the call back

///* Image/texture information */
//typedef struct {
//    int Width, Height;
//    int Format;
//    int BytesPerPixel;
//    unsigned char *Data;
//} GLFWimage;
