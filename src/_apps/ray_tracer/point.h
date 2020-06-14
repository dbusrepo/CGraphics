#pragma once
#include <math.h>
#include "real.h"
#include "vector3.r"

struct point3
{
	real x, y, z;
};

typedef struct point3 point3_t;

inline void point3_zero(point3_t *p)
{
	p->x = p->y = p->z = 0;
}

inline void point3_init(point3_t *p, real px, real py, real pz)
{
	p->x = px;
	p->y = py;
	p->z = pz;
}

inline void point3_copy(point3_t *t, point3_t *s)
{
	t->x = s->x;
	t->y = s->y;
	t->z = s->z;
}

inline void point3_negate(point3_t *r, point3_t *p)
{
	r->x = -p->x;
	r->y = -p->y;
	r->z = -p->z;
}

// the vector that joins point b to point a
inline void point3_sub(vec3_t *r, point3_t *a, point3_t *b)
{
	r->x = a->x - b->x;
	r->y = a->y - b->y;
	r->z = a->z - b->z;
}

// addition of a vector to a point that returns a new point
inline void point3_add_vec(point3_t *r, point3_t *a, vec3_t *v)
{
	r->x = a->x + v->x;
	r->y = a->y + v->y;
	r->z = a->z + v->z;
}

// subtraction of a vector from a point that returns a new point
inline void point3_sub_vec(point3_t *r, point3_t *a, vec3_t *v)
{
	r->x = a->x - v->x;
	r->y = a->y - v->y;
	r->z = a->z - v->z;
}

inline void point3_scale(point3_t *r, point3_t *p, real k)
{
	r->x = k * p->x;
	r->y = k * p->y;
	r->z = k * p->z;
}

inline real point3_dist_squared(point3_t *a, point3_t *b)
{
	return ((a->x - b->x) * (a->x - b->x)
	        + (a->y - b->y) * (a->y - b->y)
	        + (a->z - b->z) * (a->z - b->z));
}

inline real point3_distance(point3_t *a, point3_t *b)
{
	return (real) sqrt(point3_dist_squared(a, b));
}
