#pragma once
#include <math.h>

struct rgb_color
{
	float r, g, b;
};

typedef struct rgb_color rgb_color_t;

inline void rgb_zero(rgb_color_t *c)
{
	c->r = c->g = c->b = 0;
}

inline void rgb_init_3f(rgb_color_t *c, float r, float g, float b)
{
	c->r = r;
	c->g = g;
	c->b = b;
}

inline void rgb_init_f(rgb_color_t *c, float a)
{
	rgb_init_3f(c, a, a, a);
}

inline void rgb_copy(rgb_color_t *a, const rgb_color_t *b)
{
	a->r = b->r;
	a->g = b->g;
	a->b = b->b;
}

inline void rgb_add(rgb_color_t *r, rgb_color_t *a, rgb_color_t *b)
{
	r->r = a->r + b->r;
	r->g = a->g + b->g;
	r->b = a->b + b->b;
}

inline void rgb_cadd(rgb_color_t *a, rgb_color_t *b) // compound addition
{
	a->r += b->r;
	a->g += b->g;
	a->b += b->b;
}

inline void rgb_scale(rgb_color_t *r, rgb_color_t *c, float k)
{
	r->r = k * c->r;
	r->g = k * c->g;
	r->b = k * c->b;
}

inline void rgb_cscale(rgb_color_t *c, float k)
{
	c->r *= k;
	c->g *= k;
	c->b *= k;
}

inline float rgb_average(rgb_color_t *c)
{
	return (c->r + c->g + c->b) / 3;
}

inline void rgb_powc(rgb_color_t *r, rgb_color_t *c, float p)
{
	r->r = powf(c->r, p);
	r->g = powf(c->g, p);
	r->b = powf(c->b, p);
}

//inline bool rgb_same(rgb_color_t *a, rgb_color_t *b)
//{
//	return a->r == b->r && a->g == b->g && a->b == b->b;
//}
