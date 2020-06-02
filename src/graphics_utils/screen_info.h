#pragma once
#include <inttypes.h>

typedef struct screen_info screen_info_t;

struct screen_info
{
	uint32_t ext_max_red, ext_max_green, ext_max_blue;
	uint32_t bytes_per_pixel; // 4
	uint32_t bytes_per_rgb; // 3
	uint32_t red_mask, green_mask, blue_mask;
	uint32_t red_shift, green_shift, blue_shift,
			red_max, green_max, blue_max;
	uint8_t *pbuffer;
};

uint32_t ext_to_native(screen_info_t *screen_info, uint32_t red, uint32_t green, uint32_t blue);
