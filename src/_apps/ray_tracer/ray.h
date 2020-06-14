#pragma once
#include "vector.h"
#include "point.h"

struct ray
{
	point3_t o; // origin
	vec3_t d; // direction
};

typedef struct ray ray_t;

inline void ray_init_default(ray_t *r)
{
	point3_zero(&r->o);
	vec3_init(&r->d, 0, 0, 1);
}

inline void ray_init(ray_t *r, point3_t *o, vec3_t *d)
{
	point3_copy(&r->o, o);
	vec3_copy(&r->d, d);
}

inline void ray_copy(ray_t *t, ray_t *s)
{
	point3_copy(&t->o, &s->o);
	vec3_copy(&t->d, &s->d);
}

