#pragma once

typedef struct screen_info_rgb screen_info_rgb_t;

screen_info_rgb_t *init_screen_info_rgb(
        unsigned long red_mask, unsigned long green_mask, unsigned long blue_mask,
        int bytes_per_pixel, int bytes_per_rgb,
        unsigned char *p_screenbuf);
