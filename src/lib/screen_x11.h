#pragma once

typedef struct screen_x11 screen_x11_t;

screen_x11_t *init_screen_x11(int xsize, int ysize);
void run(screen_x11_t *xgraph_app);
