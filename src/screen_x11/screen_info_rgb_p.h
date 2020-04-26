#pragma once

#include "../graphics_utils/screen_info_rgb.h"

screen_info_rgb_t *init_screen_info_rgb(
        uint32_t red_mask, uint32_t green_mask, uint32_t blue_mask,
        uint8_t bytes_per_pixel, uint8_t bytes_per_rgb,
        uint8_t *buffer);
