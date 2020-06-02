#pragma once
#include "screen_info.h"

void font_init(screen_info_t *screen_info);

void draw_text(screen_info_t *screen_info, uint32_t xsize, uint32_t ysize, uint32_t x, uint32_t y, const char *text);

void set_text_color(screen_info_t *screen_info, uint32_t red, uint32_t green, uint32_t blue);
