#pragma once
#include <math.h>

#define X 0
#define Y 1
#define Z 2

typedef float real;
typedef real vec_t[3];

inline void vec_zero(vec_t v)
{
	v[X] = v[Y] = v[Z] = 0;
}

inline void vec_init(vec_t v, real vx, real vy, real vz)
{
	v[X] = vx;
	v[Y] = vy;
	v[Z] = vz;
}

inline void vec_copy(vec_t a, vec_t b)
{
	a[X] = b[X];
	a[Y] = b[Y];
	a[Z] = b[Z];
}

inline void vec_sub(vec_t r, vec_t a, vec_t b)
{
	r[X] = a[X] - b[X];
	r[Y] = a[Y] - b[Y];
	r[Z] = a[Z] - b[Z];
}

inline void vec_add(vec_t r, vec_t a, vec_t b)
{
	r[X] = a[X] + b[X];
	r[Y] = a[Y] + b[Y];
	r[Z] = a[Z] + b[Z];
}

inline void vec_cadd(vec_t a, vec_t b) // compound addition
{
	a[X] += b[X];
	a[Y] += b[Y];
	a[Z] += b[Z];
}

inline void vec_scale(vec_t r, vec_t v, real k)
{
	r[X] = k * v[X];
	r[Y] = k * v[Y];
	r[Z] = k * v[Z];
}

inline void vec_negate(vec_t r, vec_t v)
{
	r[X] = -v[X];
	r[Y] = -v[Y];
	r[Z] = -v[Z];
}

inline real vec_dot(vec_t a, vec_t b)
{
	return a[X] * b[X] + a[Y] * b[Y] + a[Z] * b[Z];
}

inline real len_squared(vec_t v)
{
	return vec_dot(v, v);
}

inline real vec_length(vec_t v)
{
	return (real) sqrt((double) vec_dot(v, v));
}

inline void vec_cross(vec_t r, vec_t a, vec_t b)
{
	r[X] = a[Y] * b[Z] - a[Z] * b[Y];
	r[Y] = a[Z] * b[X] - a[X] * b[Z];
	r[Z] = a[X] * b[Y] - a[Y] * b[X];
}

inline void vec_normalize(vec_t v)
{
	real length = vec_length(v);
	vec_scale(v, v, 1 / length);
}
