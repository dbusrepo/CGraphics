#pragma once
//#include <stdbool.h>

struct rgb
{
	float r, g, b;
};

typedef struct rgb rgb_t;

inline void rgb_zero(rgb_t *c)
{
	c->r = c->g = c->b = 0;
}

inline void rgb_color(rgb_t *c, float r, float g, float b)
{
	c->r = r;
	c->g = g;
	c->b = b;
}

inline void rgb_copy(rgb_t *a, rgb_t *b)
{
	a->r = b->r;
	a->g = b->g;
	a->b = b->b;
}

inline void rgb_add(rgb_t *r, rgb_t *a, rgb_t *b)
{
	r->r = a->r + b->r;
	r->g = a->g + b->g;
	r->b = a->b + b->b;
}

inline void rgb_cadd(rgb_t *a, rgb_t *b) // compound addition
{
	a->r += b->r;
	a->g += b->g;
	a->b += b->b;
}

inline void rgb_scale(rgb_t *r, rgb_t *c, float k)
{
	r->r = k * c->r;
	r->g = k * c->g;
	r->b = k * c->b;
}

inline void rgb_cscale(rgb_t *c, float k)
{
	c->r *= k;
	c->g *= k;
	c->b *= k;
}

inline float rgb_average(rgb_t *c)
{
	return (float) (0.333333333333 * (c->r + c->g + c->b));
}

inline void rgb_powc(rgb_t *r, rgb_t *c, float p)
{
	r->r = powf(c->r, p);
	r->g = powf(c->g, p);
	r->b = powf(c->b, p);
}

//inline bool rgb_same(rgb_t *a, rgb_t *b)
//{
//	return a->r == b->r && a->g == b->g && a->b == b->b;
//}
