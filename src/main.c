#include "../includes/rope_simu.h"

t_vector2	circle_draw_table[CIRCLE_RAY];

// assuming bits per pixel is always 32
t_img	create_image(void *mlx_ptr, int width, int height)
{
	t_img	img;
	void	*tmp;
	int		dummy;

	img = (t_img){0};
	tmp = mlx_new_image(mlx_ptr, width, height);
	if (!tmp)
		return (img);
	img.img = tmp;
	img.addr = (uint *)mlx_get_data_addr(tmp, &dummy, &img.size.x, &dummy);
	img.size.x = width;
	img.size.y = height;
	return (img);
}

void	init_circle_draw_table()
{
	for (int i = 0; i < CIRCLE_RAY; i++)
	{
		circle_draw_table[i].x = CIRCLE_RAY - sqrt((CIRCLE_RAY * CIRCLE_RAY) - (i * i));
		circle_draw_table[i].y = 2 * CIRCLE_RAY - 2 * circle_draw_table[i].x;
	}
}

void	ft_fill_color(uint *ptr, uint color, size_t size)
{
	for (unsigned int i = 0; i < size; i++)
	{
		*ptr = color;
		ptr++;
	}
}

void	draw_circle_at(t_img *img, int x, int y, uint color)
{
	y += CIRCLE_RAY;
	for (int i = 0; i < CIRCLE_RAY; i++)
	{
		if (y < HEIGHT)
			ft_fill_color(img->addr + img->size.x * y + x + circle_draw_table[i].x, color, circle_draw_table[i].y);
		if (y - CIRCLE_RAY < HEIGHT)
			ft_fill_color(img->addr + img->size.x * (y - CIRCLE_RAY) + x + circle_draw_table[CIRCLE_RAY - i - 1].x, color, circle_draw_table[CIRCLE_RAY - i - 1].y);
		y++;
	}
}

static double	get_delta_time()
{
	static struct timespec	last_time = {0};
	struct timespec			cur_time;
	double	delta;

	if (last_time.tv_sec == 0)
		clock_gettime(CLOCK_REALTIME, &last_time);
	clock_gettime(CLOCK_REALTIME, &cur_time);
	delta = (cur_time.tv_sec - last_time.tv_sec
			+ (cur_time.tv_nsec - last_time.tv_nsec) / 1000000000.F);
	last_time = cur_time;
	return (delta);
}

void	draw_circles(t_infos *infos)
{
	for (int i = 0; i < NB_MAX_CIRCLES; i++)
	{
		if (infos->circles[i].id != -1)
		{
			draw_circle_at(&infos->img, infos->circles[i].pos.x, infos->circles[i].pos.y, infos->circles[i].color);
		}
	}
}


t_dvector2	normalize(t_dvector2 vector)
{
	double	magn;

	magn = sqrt((vector.x * vector.x) + (vector.y * vector.y));
	if (magn == 0)
		return ((t_dvector2){0, 0});
	return ((t_dvector2){vector.x / magn, vector.y / magn});
}

void	apply_forces(t_infos *infos)
{
	for (int i = 0; i < NB_MAX_CIRCLES; i++)
	{
		if (infos->circles[i].id != -1 && !infos->circles[i].is_locked)
		{
			t_dvector2 pos_tmp = infos->circles[i].pos;
			infos->circles[i].pos.x += (infos->circles[i].pos.x - infos->circles[i].prev_pos.x) * AIR_FRICTION;
			infos->circles[i].pos.y += (infos->circles[i].pos.y - infos->circles[i].prev_pos.y) * AIR_FRICTION;

			infos->circles[i].pos.y += GRAVITY * infos->delta_time * AIR_FRICTION;


			infos->circles[i].prev_pos = pos_tmp;
		}
	}

	for (int iter = 0; iter < 100; iter++)
	{
		for (int i = 0; i < NB_MAX_CIRCLES; i++)
		{
			t_circle *circles = infos->circles;
			for (int j = 0; j < NB_MAX_LINKS; j++)
			{
				if (circles[i].links[j].right_id != -1 && circles[i].links[j].right_id < circles[i].links[j].left_id)
				{
					t_link *link = &circles[i].links[j];

					t_dvector2 link_center =
						{(circles[link->right_id].pos.x + circles[link->left_id].pos.x) / 2,
						(circles[link->right_id].pos.y + circles[link->left_id].pos.y) / 2};

					t_dvector2 link_dir = normalize((t_dvector2){
						circles[link->right_id].pos.x - circles[link->left_id].pos.x,
						circles[link->right_id].pos.y - circles[link->left_id].pos.y
					});
					
					if (!circles[link->right_id].is_locked)
					{
						circles[link->right_id].pos.x = link_center.x + link_dir.x * (link->length / 2);
						circles[link->right_id].pos.y = link_center.y + link_dir.y * (link->length / 2);
					}

					if (!circles[link->left_id].is_locked)
					{
						circles[link->left_id].pos.x = link_center.x - (link_dir.x * (link->length / 2));
						circles[link->left_id].pos.y = link_center.y - (link_dir.y * (link->length / 2));
					}
				}
			}
		}
	}

}

void	delete_lost_circles(t_infos *infos)
{
	for (int i = 0; i < NB_MAX_CIRCLES; i++)
	{
		if (infos->circles[i].id != -1 && infos->circles[i].pos.y > HEIGHT)
		{
			memset(&infos->circles[i], 0, sizeof(t_circle));
			infos->circles[i].id = -1;
		}
	}
}


void	draw_line(t_vector2 p1, t_vector2 p2, t_img *img)
{
	t_dvector2	dir;
	int			dist;

	dist = sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
	dir.x = (double)(p2.x - p1.x) / dist;
	dir.y = (double)(p2.y - p1.y) / dist;
	for (int i = 0; i < dist; i++)
	{
		*(img->addr + ((int)(p1.y + i * dir.y) * HEIGHT) + (int)(p1.x + i * dir.x)) = 0xFF0000;
	}
}

void	draw_links(t_infos *infos)
{
	for (int i = 0; i < NB_MAX_CIRCLES; i++)
	{
		if (infos->circles[i].id == -1)
			continue ;
		for (int j = 0; j < NB_MAX_LINKS; j++)
		{
			if (infos->circles[i].links[j].right_id != -1 && infos->circles[i].links[j].right_id < infos->circles[i].id)
			{
				t_vector2 p1, p2;
				p1.x = infos->circles[infos->circles[i].links[j].right_id].pos.x + CIRCLE_RAY;
				p1.y = infos->circles[infos->circles[i].links[j].right_id].pos.y + CIRCLE_RAY;

				p2.x = infos->circles[i].pos.x + CIRCLE_RAY;
				p2.y = infos->circles[i].pos.y + CIRCLE_RAY;
				draw_line(p1, p2, &infos->img);
			}
		}
	}
}

int	on_update(t_infos *infos)
{
	static int cmp = 0;

	infos->delta_time = get_delta_time();
	if (infos->is_pause)
	{
		draw_circles(infos);
		mlx_put_image_to_window(infos->mlx_ptr, infos->window, infos->img.img, 0, 0);
		return (0);
	}
	cmp++;
	if (cmp % 500 == 0)
	{
		printf("fps %lf\n", 1 / infos->delta_time);
		cmp = 0;
	}
	memset(infos->img.addr, 0x35, infos->img.size.x * infos->img.size.y * 4);
	apply_forces(infos);
	draw_circles(infos);
	draw_links(infos);
	delete_lost_circles(infos);
	mlx_put_image_to_window(infos->mlx_ptr, infos->window, infos->img.img, 0, 0);


	return (0);
}

int main(void)
{
	t_infos	infos = {0};

	infos.mlx_ptr = mlx_init();
	if (!infos.mlx_ptr)
		return (1); // error
	infos.window = mlx_new_window(infos.mlx_ptr, WIDTH, HEIGHT, "rope_simu");
	if (!infos.window)
		return (1); // error
	infos.img = create_image(infos.mlx_ptr, WIDTH, HEIGHT);
	if (!infos.img.addr)
		return (1); // error
	infos.is_pause = true;
	memset(infos.circles, -1, NB_MAX_CIRCLES * sizeof(t_circle));
	init_circle_draw_table();
	memset(infos.img.addr, 0x35, infos.img.size.x * infos.img.size.y * 4);

	mlx_mouse_hook(infos.window, click_mouse, &infos);
	mlx_hook(infos.window, 2, (1L << 0), key_press_hook, &infos);
	mlx_loop_hook(infos.mlx_ptr, on_update, &infos);
	mlx_loop(infos.mlx_ptr);
	return (0);
}