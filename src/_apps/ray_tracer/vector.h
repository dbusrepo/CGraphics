#pragma once
#include <math.h>

#define X 0
#define Y 1
#define Z 2

typedef float real;

struct vec2
{
	real x, y;
};

typedef struct vec2 vec2_t;

struct vec3
{
	real x, y, z;
};

typedef struct vec3 vec3_t;

/**** vec2_t ****/

inline void vec2_zero(vec2_t *v)
{
	v->x = v->y = 0;
}

inline void vec2_init(vec2_t *v, real vx, real vy)
{
	v->x = vx;
	v->y = vy;
}

inline void vec2_copy(vec2_t *a, vec2_t *b)
{
	a->x = b->x;
	a->y = b->y;
}

inline void vec2_negate(vec2_t *r, vec2_t *v)
{
	r->x = -v->x;
	r->y = -v->y;
}

inline void vec2_add(vec2_t *r, vec2_t *a, vec2_t *b)
{
	r->x = a->x + b->x;
	r->y = a->y + b->y;
}

inline void vec2_sub(vec2_t *r, vec2_t *a, vec2_t *b)
{
	r->x = a->x - b->x;
	r->y = a->y - b->y;
}

inline void vec2_scale(vec2_t *r, vec2_t *v, real k)
{
	r->x = k * v->x;
	r->y = k * v->y;
}

inline real vec2_dot(vec2_t *a, vec2_t *b)
{
	return a->x * b->x + a->y * b->y;
}

inline real vec2_len_squared(vec2_t *v)
{
	return vec2_dot(v, v);
}

inline void vec2_normal(vec2_t *n, vec2_t *v)
{
	n->x = -v->y;
	n->y = v->x;
}

inline void vec2_linear_comb(vec2_t *r, vec2_t *p, vec2_t *pq, real lambda)
{
	r->x = p->x + lambda * pq->x;
	r->y = p->y + lambda * pq->y;
}

inline void vec2_point_on_line(vec2_t *r, vec2_t *p, vec2_t *q, real lambda)
{
	r->x = p->x + lambda * (q->x - p->x);
	r->y = p->y + lambda * (q->y - p->y);
}

/**** vec3_t  * ****/

inline void vec3_zero(vec3_t *v)
{
	v->x = v->y = v->z = 0;
}

inline void vec3_init(vec3_t *v, real vx, real vy, real vz)
{
	v->x = vx;
	v->y = vy;
	v->z = vz;
}

inline void vec3_copy(vec3_t *a, vec3_t *b)
{
	a->x = b->x;
	a->y = b->y;
	a->z = b->z;
}

inline void vec3_sub(vec3_t *r, vec3_t *a, vec3_t *b)
{
	r->x = a->x - b->x;
	r->y = a->y - b->y;
	r->z = a->z - b->z;
}

inline void vec3_add(vec3_t *r, vec3_t *a, vec3_t *b)
{
	r->x = a->x + b->x;
	r->y = a->y + b->y;
	r->z = a->z + b->z;
}

inline void vec3_cadd(vec3_t *a, vec3_t *b) // compound addition
{
	a->x += b->x;
	a->y += b->y;
	a->z += b->z;
}

inline void vec3_scale(vec3_t *r, vec3_t *v, real k)
{
	r->x = k * v->x;
	r->y = k * v->y;
	r->z = k * v->z;
}

inline void vec3_negate(vec3_t *r, vec3_t *v)
{
	r->x = -v->x;
	r->y = -v->y;
	r->z = -v->z;
}

inline real vec3_dot(vec3_t *a, vec3_t *b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

inline real vec3_len_squared(vec3_t *v)
{
	return vec3_dot(v, v);
}

inline real vec3_length(vec3_t *v)
{
	return (real) sqrt((double) vec3_dot(v, v));
}

inline void vec3_cross(vec3_t *r, vec3_t *a, vec3_t *b)
{
	r->x = a->y * b->z - a->z * b->y;
	r->y = a->z * b->x - a->x * b->z;
	r->z = a->x * b->y - a->y * b->x;
}

inline void vec3_normalize(vec3_t *v)
{
	real length = vec3_length(v);
	vec3_scale(v, v, 1 / length);
}

inline void vec3_linear_comb(vec3_t *r, vec3_t *p, vec3_t *pq, real lambda)
{
	r->x = p->x + lambda * pq->x;
	r->y = p->y + lambda * pq->y;
	r->z = p->z + lambda * pq->z;
}

inline void vec3_point_on_line(vec3_t *r, vec3_t *p, vec3_t *q, real lambda)
{
	r->x = p->x + lambda * (q->x - p->x);
	r->y = p->y + lambda * (q->y - p->y);
	r->z = p->z + lambda * (q->z - p->z);
}