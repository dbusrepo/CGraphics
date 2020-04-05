#include <stdio.h>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>

#include "lib/screen_x11.h"

int main(int argc, char *argv[]) {
    screen_x11_t *screen_x11 = init_screen_x11(800, 600);
    run(screen_x11);
    return 0;
}
