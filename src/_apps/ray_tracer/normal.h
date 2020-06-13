#pragma once
#include "real.h"
#include "vector.h"

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

inline void normal_copy(normal_t *a, normal_t *b)
{
	a->x = b->x;
	a->y = b->y;
	a->z = b->z;
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

/* normal & vec */

inline void vec3_normal_add(vec3_t *r, vec3_t *v, normal_t *n)
{
	r->x = n->x + v->x;
	r->y = n->y + v->y;
	r->z = n->z + v->z;
}

inline void vec3_normal_sub(vec3_t *r, vec3_t *v, normal_t *n)
{
	r->x = v->x - n->x;
	r->y = v->y - n->y;
	r->z = v->z - n->z;
}

// dot product of n normal on the left and n vector on the right
inline real vec3_normal_dot(vec3_t *n, normal_t *v)
{
	return n->x * v->x + n->y * v->y + n->z * v->z;
}

