#pragma once

#include "../graphics_utils/screen_settings.h"

typedef void (*update_fptr_t)(int64_t elapsed_time);
typedef void (*render_fptr_t)(void);
typedef void (*key_event_fptr_t)(int key_code);

typedef struct app app_t;

app_t *init_app(screen_settings_t *screen_settings);

void run_event_loop(app_t *app,
                    update_fptr_t update_fptr,
                    render_fptr_t render_fptr,
                    key_event_fptr_t key_event_fptr);

