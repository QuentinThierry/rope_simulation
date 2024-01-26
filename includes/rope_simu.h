#ifndef ROPE_SIMU_H
#define ROPE_SIMU_H

#include "./minilibx-linux/mlx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
# include <X11/keysym.h>

#define	CIRCLE_RAY 32
#define	NB_MAX_CIRCLES 32

#define	GRAVITY 1 // pixels per second

typedef unsigned int uint;

typedef struct s_vector2
{
	int x;
	int y;
}	t_vector2;

typedef struct s_dvector2
{
	double x;
	double y;
}	t_dvector2;

typedef struct s_img
{
	void		*img;
	uint		*addr;
	t_vector2	size;
}	t_img;

typedef struct s_circle
{
	t_dvector2	pos;
	int			id;
	t_dvector2	force;
}	t_circle;

typedef struct s_infos
{
	void		*mlx_ptr;
	void		*window;
	t_img		img;
	double		delta_time;
	t_circle	circles[NB_MAX_CIRCLES];
}	t_infos;

extern t_vector2	circle_table[CIRCLE_RAY];


int		click_mouse(int key_code, int x, int y, t_infos *infos);
void	draw_circle_at(t_img *img, int x, int y);
int		key_press_hook(int key_code, t_infos *infos);

#endif
