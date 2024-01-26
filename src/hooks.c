#include "../includes/rope_simu.h"

int	key_press_hook(int key_code, t_infos *infos)
{
	if (key_code == XK_Escape)
		exit(0);
	return (0);
}

int	click_mouse(int key_code, int x, int y, t_infos *infos)
{
	if (key_code == 1)
	{
		x -= CIRCLE_RAY;
		y -= CIRCLE_RAY;
		if (x < 0)
			x = 0;
		if (y < 0)
			y = 0;
		for (int i = 0; i < NB_MAX_CIRCLES; i++)
		{
			if (infos->circles[i].id == -1)
			{
				draw_circle_at(&infos->img, x, y);
				infos->circles[i].id = i;
				infos->circles[i].pos.x = x;
				infos->circles[i].pos.y = y;
				infos->circles[i].force.x = 0;
				infos->circles[i].force.y = 0;
				break ;
			}
		}
	}
	return (0);
}
