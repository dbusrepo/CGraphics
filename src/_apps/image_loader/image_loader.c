#include "image_loader.h"
#include <stdlib.h>
#include "../app/app.h"

static app_t *app;

void update(int64_t elapsed_time) {
}

void render(void) {
}

void key_event(int key, int action) {
    if (action == KEY_PRESS) {
        switch (key) {
            case KEY_ESC: {
                app_terminate(app);
                break;
            }
            case KEY_F1: {
                app_toggle_fullscreen(app);
                break;
            }
        }
    }
}

int image_loader_run() {

    screen_settings_t *screen_settings = malloc(sizeof(screen_settings));
    screen_settings->window_title = "Image Loader";
    screen_settings->width = 1024;
    screen_settings->height = 768;
    screen_settings->targetFps = 60;
    screen_settings->show_stats = true;
    screen_settings->fullscreen = false;
    app = app_init(screen_settings);
    app_run(app, update, render, key_event);
    return 0;
}
