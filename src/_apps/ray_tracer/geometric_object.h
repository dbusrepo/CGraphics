#pragma once
#include "rgb_color.h"

struct geometric_object_vtable
{
	void (*hit)();
};

struct geometric_object
{
	rgb_color_t color;    // only used for Bare Bones ray tracing
};

