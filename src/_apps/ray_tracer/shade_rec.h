#pragma once
#include "normal.h"
#include "point.h"
#include "rgb_color.h"
#include "world.h"
#include <stdbool.h>

struct shade_rec
{
	bool hit_an_object;       // did the ray hit an object?
	point3_t world_hit_point; // world coordinates of hit point on untransformed object (used for texture transformations)
	normal_t normal;          // normal at hit point
	rgb_color_t color;        // used in the Chapter 3 only
	world_t *world;           // world reference
};

typedef struct shade_rec shade_rec_t;

inline void shade_rec_init(shade_rec_t *sr, world_t *world)
{
	sr->hit_an_object = false;
	point3_zero(&sr->world_hit_point);
	normal_zero(&sr->normal);
	rgb_copy(&sr->color, &BLACK);
	sr->world = world;
}

inline void shade_rec_copy(shade_rec_t *t, shade_rec_t *s)
{
	t->hit_an_object = s->hit_an_object;
	point3_copy(&t->world_hit_point, &s->world_hit_point);
	normal_copy(&t->normal, &s->normal);
	rgb_copy(&t->color, &s->color);
	t->world = s->world;
}
