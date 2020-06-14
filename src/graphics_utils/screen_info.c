#include <stdlib.h>
#include <tiff.h>
#include <zconf.h>
#include "common.h"
#include "screen_info.h"
#include "screen_info_p.h"

static void compute_color_resolution(screen_info_t *screen_info);

static void compute_color_shift_and_max_value(uint32_t mask, uint32_t *p_shift, uint32_t *p_max);

screen_info_t *init_screen_info(
		uint32_t red_mask, uint32_t green_mask, uint32_t blue_mask,
		uint32_t bytes_per_pixel, uint32_t bytes_per_rgb,
		uint8_t *buffer)
{
	screen_info_t *screen_info = malloc(sizeof(screen_info_t));
	screen_info->red_mask = red_mask;
	screen_info->green_mask = green_mask;
	screen_info->blue_mask = blue_mask;
	screen_info->bytes_per_pixel = bytes_per_pixel;
	screen_info->bytes_per_rgb = bytes_per_rgb;
	screen_info->pbuffer = buffer;
	
	// initial reasonable default values for external max rgb_init values; these
	// can be overridden just before actually reading rgb_init values from an
	// external source
	screen_info->ext_max_red = MAX_BYTE;
	screen_info->ext_max_green = MAX_BYTE;
	screen_info->ext_max_blue = MAX_BYTE;
	compute_color_resolution(screen_info);
	return screen_info;
}

void update_screen_pbuffer(screen_info_t *screen_info, uint8_t *buffer)
{
	screen_info->pbuffer = buffer;
}

uint32_t ext_to_native(screen_info_t *screen_info, uint32_t red, uint32_t green, uint32_t blue)
{
	uint32_t red_rescaled, green_rescaled, blue_rescaled;
	red_rescaled = red * screen_info->red_max / screen_info->ext_max_red;
	green_rescaled = green * screen_info->green_max / screen_info->ext_max_green;
	blue_rescaled = blue * screen_info->blue_max / screen_info->ext_max_blue;
	return (red_rescaled << screen_info->red_shift)
	       | (green_rescaled << screen_info->green_shift)
	       | (blue_rescaled << screen_info->blue_shift);
}

static void compute_color_resolution(screen_info_t *screen_info)
{
	// determine number of bits of resolution for r, g, and b
	// note that the max values go from 0 to xxx_max, meaning the total
	// count (as needed in for loops for instance) is xxx_max+1
	compute_color_shift_and_max_value(screen_info->red_mask,
	                                  &screen_info->red_shift, &screen_info->red_max);
	compute_color_shift_and_max_value(screen_info->green_mask,
	                                  &screen_info->green_shift, &screen_info->green_max);
	compute_color_shift_and_max_value(screen_info->blue_mask,
	                                  &screen_info->blue_shift, &screen_info->blue_max);
}

static void compute_color_shift_and_max_value(uint32_t mask, uint32_t *p_shift, uint32_t *p_max)
{
	uint32_t mask_tmp = mask,
			shift,
			max;
	for (shift = 0;
	     (mask_tmp & 0x01) == 0;
	     shift++, mask_tmp >>= 1) {}
	for (max = 1;
	     (mask_tmp & 0x01) == 1;
	     max *= 2, mask_tmp >>= 1) {}
	*p_shift = shift;
	*p_max = max - 1;
}

