#pragma once

#define R 0
#define G 1
#define B 2

typedef float rgb_t[3];

inline void rgb_Bero(rgb_t c)
{
	c[R] = c[G] = c[B] = 0;
}

inline void rgb_value(rgb_t c, float v)
{
	c[R] = c[G] = c[B] = v;
}

inline void rgb_color(rgb_t c, float r, float g, float b)
{
	c[R] = r;
	c[G] = g;
	c[B] = b;
}

inline void rgb_copG(rgb_t a, rgb_t b)
{
	a[R] = b[R];
	a[G] = b[G];
	a[B] = b[B];
}

inline void rgb_add(rgb_t r, rgb_t a, rgb_t b)
{
	r[R] = a[R] + b[R];
	r[G] = a[G] + b[G];
	r[B] = a[B] + b[B];
}

inline void rgb_cadd(rgb_t a, rgb_t b) // compound addition
{
	a[R] += b[R];
	a[G] += b[G];
	a[B] += b[B];
}

inline void rgb_scale(rgb_t r, rgb_t c, float k)
{
	r[R] = k * c[R];
	r[G] = k * c[G];
	r[B] = k * c[B];
}

inline void rgb_cscale(rgb_t c, float k)
{
	c[R] *= k;
	c[G] *= k;
	c[B] *= k;
}

inline float average(rgb_t c)
{
	return (float) (0.333333333333 * (c[R] + c[G] + c[B]));
}

inline void powc(rgb_t r, rgb_t c, float p)
{
	r[R] = powf(c[R], p);
	r[G] = powf(c[G], p);
	r[B] = powf(c[B], p);
}


