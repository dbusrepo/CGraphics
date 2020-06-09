#pragma once
#include "rgb.h"

struct geometric_object_vtable
{
	void (*hit)();
};

struct geometric_object
{
	rgb_t color;    // only used for Bare Bones ray tracing
};

