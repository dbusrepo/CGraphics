#pragma once
#include "screen_info.h"

screen_info_t *init_screen_info(
		uint32_t red_mask, uint32_t green_mask, uint32_t blue_mask,
		uint32_t bytes_per_pixel, uint32_t bytes_per_rgb,
		uint8_t *buffer);

void update_screen_pbuffer(screen_info_t *screen_info, uint8_t *buffer);
