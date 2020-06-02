#pragma once
#include <inttypes.h>
#include <stdbool.h>

typedef struct screen_settings
{
	const char *window_title;
	uint32_t width;
	uint32_t height;
	int targetFps;
	bool show_stats;
	bool fullscreen;
} screen_settings_t;
