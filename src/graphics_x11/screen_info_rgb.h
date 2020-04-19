#pragma once

typedef struct screen_info_rgb screen_info_rgb_t;

screen_info_rgb_t *init_screen_info_rgb(
        unsigned long red_mask, unsigned long green_mask, unsigned long blue_mask,
        char bytes_per_pixel, char bytes_per_rgb,
        unsigned char *p_screenbuf);