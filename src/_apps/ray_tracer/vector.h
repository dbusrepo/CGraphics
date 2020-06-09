#pragma once
#include <math.h>

#define X 0
#define Y 1
#define Z 2

typedef float real;

typedef real vec2_t[2];
typedef real vec3_t[3];

/**** vec2_t ****/

inline void vec2_zero(vec2_t v)
{
	v[X] = v[Y] = 0;
}

inline void vec2_init(vec2_t v, real vx, real vy)
{
	v[X] = vx;
	v[Y] = vy;
}

inline void vec2_copy(vec2_t a, vec2_t b)
{
	a[X] = b[X];
	a[Y] = b[Y];
}

inline void vec2_negate(vec2_t r, vec2_t v)
{
	r[X] = -v[X];
	r[Y] = -v[Y];
}

inline void vec2_add(vec2_t r, vec2_t a, vec2_t b)
{
	r[X] = a[X] + b[X];
	r[Y] = a[Y] + b[Y];
}

inline void vec2_sub(vec2_t r, vec2_t a, vec2_t b)
{
	r[X] = a[X] - b[X];
	r[Y] = a[Y] - b[Y];
}

inline void vec2_scale(vec2_t r, vec2_t v, real k)
{
	r[X] = k * v[X];
	r[Y] = k * v[Y];
}

inline real vec2_dot(vec2_t a, vec2_t b)
{
	return a[X] * b[X] + a[Y] * b[Y];
}

inline real vec2_len_squared(vec2_t v)
{
	return vec2_dot(v, v);
}

inline void vec2_normal(vec2_t n, vec2_t v)
{
	n[X] = -v[Y];
	n[Y] = v[X];
}

inline void vec2_linear_comb(vec2_t r, vec2_t p, vec2_t pq, real lambda)
{
	r[X] = p[X] + lambda * pq[X];
	r[Y] = p[Y] + lambda * pq[Y];
}

inline void vec2_point_on_line(vec2_t r, vec2_t p, vec2_t q, real lambda)
{
	r[X] = p[X] + lambda * (q[X] - p[X]);
	r[Y] = p[Y] + lambda * (q[Y] - p[Y]);
}

/**** vec3_t ****/

inline void vec3_zero(vec3_t v)
{
	v[X] = v[Y] = v[Z] = 0;
}

inline void vec3_init(vec3_t v, real vx, real vy, real vz)
{
	v[X] = vx;
	v[Y] = vy;
	v[Z] = vz;
}

inline void vec3_copy(vec3_t a, vec3_t b)
{
	a[X] = b[X];
	a[Y] = b[Y];
	a[Z] = b[Z];
}

inline void vec3_sub(vec3_t r, vec3_t a, vec3_t b)
{
	r[X] = a[X] - b[X];
	r[Y] = a[Y] - b[Y];
	r[Z] = a[Z] - b[Z];
}

inline void vec3_add(vec3_t r, vec3_t a, vec3_t b)
{
	r[X] = a[X] + b[X];
	r[Y] = a[Y] + b[Y];
	r[Z] = a[Z] + b[Z];
}

inline void vec3_cadd(vec3_t a, vec3_t b) // compound addition
{
	a[X] += b[X];
	a[Y] += b[Y];
	a[Z] += b[Z];
}

inline void vec3_scale(vec3_t r, vec3_t v, real k)
{
	r[X] = k * v[X];
	r[Y] = k * v[Y];
	r[Z] = k * v[Z];
}

inline void vec3_negate(vec3_t r, vec3_t v)
{
	r[X] = -v[X];
	r[Y] = -v[Y];
	r[Z] = -v[Z];
}

inline real vec3_dot(vec3_t a, vec3_t b)
{
	return a[X] * b[X] + a[Y] * b[Y] + a[Z] * b[Z];
}

inline real vec3_len_squared(vec3_t v)
{
	return vec3_dot(v, v);
}

inline real vec3_length(vec3_t v)
{
	return (real) sqrt((double) vec3_dot(v, v));
}

inline void vec3_cross(vec3_t r, vec3_t a, vec3_t b)
{
	r[X] = a[Y] * b[Z] - a[Z] * b[Y];
	r[Y] = a[Z] * b[X] - a[X] * b[Z];
	r[Z] = a[X] * b[Y] - a[Y] * b[X];
}

inline void vec3_normalize(vec3_t v)
{
	real length = vec3_length(v);
	vec3_scale(v, v, 1 / length);
}

inline void vec3_linear_comb(vec3_t r, vec3_t p, vec3_t pq, real lambda)
{
	r[X] = p[X] + lambda * pq[X];
	r[Y] = p[Y] + lambda * pq[Y];
	r[Z] = p[Z] + lambda * pq[Z];
}

inline void vec3_point_on_line(vec3_t r, vec3_t p, vec3_t q, real lambda)
{
	r[X] = p[X] + lambda * (q[X] - p[X]);
	r[Y] = p[Y] + lambda * (q[Y] - p[Y]);
	r[Z] = p[Z] + lambda * (q[Z] - p[Z]);
}
