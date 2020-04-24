#include <stdlib.h>
#include "screen_info_rgb.h"

struct screen_info_rgb {
    int ext_max_red, ext_max_green, ext_max_blue;
    int bytes_per_pixel; // 4
    int bytes_per_rgb; // 3
    unsigned long red_mask, green_mask, blue_mask;
    int red_shift, green_shift, blue_shift,
        red_max, green_max, blue_max;
    unsigned char *screenbuf;
};

static void compute_color_resolution(screen_info_rgb_t *screen_info_rgb);
static void compute_color_shift_and_max_value(int mask, int *p_shift, int *p_max);

screen_info_rgb_t *init_screen_info_rgb(
        unsigned long red_mask, unsigned long green_mask, unsigned long blue_mask,
        int bytes_per_pixel, int bytes_per_rgb,
        unsigned char *p_screenbuf) {
    screen_info_rgb_t *screen_info_rgb = malloc(sizeof(screen_info_rgb_t));

    screen_info_rgb->red_mask = red_mask;
    screen_info_rgb->green_mask = green_mask;
    screen_info_rgb->blue_mask = blue_mask;
    screen_info_rgb->bytes_per_pixel = bytes_per_pixel;
    screen_info_rgb->bytes_per_rgb = bytes_per_rgb;

    // initial reasonable default values for external max rgb values; these
    // can be overridden just before actually reading rgb values from an
    // external source
    screen_info_rgb->ext_max_red = 255;
    screen_info_rgb->ext_max_green = 255;
    screen_info_rgb->ext_max_blue = 255;

    compute_color_resolution(screen_info_rgb);

    return screen_info_rgb;
}


void compute_color_resolution(screen_info_rgb_t *screen_info_rgb) {
    // determine number of bits of resolution for r, g, and b
    // note that the max values go from 0 to xxx_max, meaning the total
    // count (as needed in for loops for instance) is xxx_max+1
    compute_color_shift_and_max_value(screen_info_rgb->red_mask,
                                      &screen_info_rgb->red_shift, &screen_info_rgb->red_max);
    compute_color_shift_and_max_value(screen_info_rgb->green_mask,
                                      &screen_info_rgb->green_shift, &screen_info_rgb->green_max);
    compute_color_shift_and_max_value(screen_info_rgb->blue_mask,
                                      &screen_info_rgb->blue_shift, &screen_info_rgb->blue_max);
}

void compute_color_shift_and_max_value(int mask, int *p_shift, int *p_max) {
    int mask_tmp = mask;
    int shift;
    int max;
    for(shift=0;
        (mask_tmp & 0x01) == 0 ;
        shift++, mask_tmp>>=1);
    for(max=1;
        (mask_tmp & 0x01) == 1 ;
        max*=2, mask_tmp>>=1);
    *p_shift = shift;
    *p_max = max-1;
}

