#pragma once
#include "real.h"
#include "assert.h"
#include "constants.h"

struct normal
{
	real x, y, z;
};

typedef struct normal normal_t;

inline void normal_zero(normal_t *n)
{
	n->x = n->y = n->z = 0;
}

inline void normal_init(normal_t *n, real nx, real ny, real nz)
{
	n->x = nx;
	n->y = ny;
	n->z = nz;
}

inline void normal_copy(normal_t *t, normal_t *s)
{
	t->x = s->x;
	t->y = s->y;
	t->z = s->z;
}

inline void normal_from_vec(normal_t *n, normal_t *v)
{
	n->x = v->x;
	n->y = v->y;
	n->z = v->z;
}

inline void normal_negate(normal_t *r, normal_t *n)
{
	r->x = -n->x;
	r->y = -n->y;
	r->z = -n->z;
}

inline void normal_add(normal_t *r, normal_t *a, normal_t *b)
{
	r->x = a->x + b->x;
	r->y = a->y + b->y;
	r->z = a->z + b->z;
}

inline void normal_cadd(normal_t *a, normal_t *b)
{
	a->x += b->x;
	a->y += b->y;
	a->z += b->z;
}

inline void normal_scale(normal_t *r, normal_t *n, real k)
{
	r->x = k * n->x;
	r->y = k * n->y;
	r->z = k * n->z;
}

inline void normal_normalize(normal_t *n)
{
	real len = (real) sqrt(n->x * n->x + n->y * n->y + n->z * n->z);
	assert(len >= EPS);
	n->x /= len;
	n->y /= len;
	n->z /= len;
}

