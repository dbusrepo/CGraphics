#include <stdio.h>
#include <stdlib.h>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>

//#include "graphics_utils/screen_info_rgb.h"
#include "app/app.h"

static app_t *app;

void update(int64_t elapsed_time) {
}

void render(void) {
}

void key_event(int key, int action) {
    if (action == KEY_PRESS) {
        switch (key) {
            case KEY_ESC: {
                terminate_app(app);
                break;
            }
            case KEY_F1: {
                toggle_fullscreen_app(app);
                break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
//    setbuf(stdout, NULL);
    screen_settings_t *screen_settings = malloc(sizeof(screen_settings));
    screen_settings->window_title = "Window Example";
    screen_settings->width = 1024;
    screen_settings->height = 768;
    screen_settings->targetFps = 60;
    screen_settings->show_stats = true;
    screen_settings->fullscreen = false;
    app = init_app(screen_settings);
    run_app(app, update, render, key_event);
    return 0;
}
