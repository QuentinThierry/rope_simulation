#include "../includes/rope_simu.h"

t_vector2	circle_table[CIRCLE_RAY];

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

void	init_circle_table()
{
	for (int i = 0; i < CIRCLE_RAY; i++)
	{
		circle_table[i].x = CIRCLE_RAY - sqrt((CIRCLE_RAY * CIRCLE_RAY) - (i * i));
		circle_table[i].y = 2 * CIRCLE_RAY - 2 * circle_table[i].x;
	}
}

void	draw_circle_at(t_img *img, int x, int y)
{
	y += CIRCLE_RAY;
	for (int i = 0; i < CIRCLE_RAY; i++)
	{
		memset(img->addr + img->size.x * y + x + circle_table[i].x, 0x50, circle_table[i].y * 4);
		memset(img->addr + img->size.x * (y - CIRCLE_RAY) + x + circle_table[CIRCLE_RAY - i - 1].x, 0x50, circle_table[CIRCLE_RAY - i - 1].y * 4);
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
			draw_circle_at(&infos->img, infos->circles[i].pos.x, infos->circles[i].pos.y);
		}
	}
}

void	calculate_forces(t_infos *infos)
{
	for (int i = 0; i < NB_MAX_CIRCLES; i++)
	{
		if (infos->circles[i].id != -1)
		{
			infos->circles[i].force.y += GRAVITY * infos->delta_time;
		}
	}
}

void	apply_forces(t_infos *infos)
{
	for (int i = 0; i < NB_MAX_CIRCLES; i++)
	{
		if (infos->circles[i].id != -1)
		{
			infos->circles[i].pos.x += infos->circles[i].force.x;
			infos->circles[i].pos.y += infos->circles[i].force.y;
		}
	}
}

int	on_update(t_infos *infos)
{
	static int cmp = 0;

	infos->delta_time = get_delta_time();
	cmp++;
	if (cmp % 500 == 0)
	{
		printf("%lf\n", 1 / infos->delta_time);
		cmp = 0;
	}
	memset(infos->img.addr, 0x35, infos->img.size.x * infos->img.size.y * 4);
	calculate_forces(infos);
	apply_forces(infos);
	draw_circles(infos);
	mlx_put_image_to_window(infos->mlx_ptr, infos->window, infos->img.img, 0, 0);


	return (0);
}

int main(void)
{
	t_infos	infos = {0};

	infos.mlx_ptr = mlx_init();
	if (!infos.mlx_ptr)
		return (1); // error
	infos.window = mlx_new_window(infos.mlx_ptr, 1000, 1000, "rope_simu");
	if (!infos.window)
		return (1); // error
	infos.img = create_image(infos.mlx_ptr, 1000, 1000);
	if (!infos.img.addr)
		return (1); // error
	memset(infos.circles, -1, NB_MAX_CIRCLES * sizeof(t_circle));
	init_circle_table();
	memset(infos.img.addr, 0x35, infos.img.size.x * infos.img.size.y * 4);

	mlx_mouse_hook(infos.window, click_mouse, &infos);
	mlx_hook(infos.window, 2, (1L << 0), key_press_hook, &infos);
	mlx_loop_hook(infos.mlx_ptr, on_update, &infos);
	mlx_loop(infos.mlx_ptr);
	return (0);
}