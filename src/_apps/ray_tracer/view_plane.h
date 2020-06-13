#pragma once

#define HRES 400
#define VRES 400
#define PIXEL_SIZE 1
#define GAMMA 1.0
#define INV_GAMMA (1.0/GAMMA)

struct view_plane
{
	int hres;                                       // horizontal image resolution
	int vres;                                       // vertical image resolution
	float s;                                        // pixel size
	float gamma;                                    // gamma correction factor
	float inv_gamma;                                // the inverse of the gamma correction factor
};

typedef struct view_plane view_plane_t;

inline void view_plane_init(view_plane_t *view_plane)
{
	view_plane->hres = HRES;
	view_plane->vres = VRES;
	view_plane->s = PIXEL_SIZE;
	view_plane->gamma = GAMMA;
	view_plane->inv_gamma = INV_GAMMA;
}

inline void view_plane_set_gamma(view_plane_t *view_plane, float g)
{
	view_plane->gamma = g;
	view_plane->inv_gamma = 1.0f / g;
}
