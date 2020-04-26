#include <stdio.h>
#include <stdlib.h>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>

//#include "graphics_utils/screen_info_rgb.h"
#include "app/app.h"

void update(int64_t elapsed_time) {
}

void render(void) {
}

void key_event(int key_code) {
}

int main(int argc, char *argv[]) {
    screen_settings_t *screen_settings = malloc(sizeof(screen_settings));
    screen_settings->window_title = "Window Example";
    screen_settings->xsize = 800;
    screen_settings->ysize = 600;
    screen_settings->targetFps = 60;
    screen_settings->show_rendering_info = true;
    app_t *app = init_app(screen_settings);
    run_event_loop(app, update, render, key_event);
    return 0;
}
