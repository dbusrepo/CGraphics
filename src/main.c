#include <stdio.h>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>

#include "graphics_x11/screen_x11.h"

int main(int argc, char *argv[]) {
    screen_x11_t *screen_x11 = init_screen_x11("Window Example", 800, 600);
    run_event_loop(screen_x11, NULL, NULL, NULL, NULL);
    return 0;
}
