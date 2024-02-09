#include "../includes/rope_simu.h"

int	key_press_hook(int key_code, t_infos *infos)
{
	if (key_code == XK_Escape)
		exit(0);
	if (key_code == ' ')
		infos->is_pause = !infos->is_pause;
	return (0);
}

static void	add_circle_to_pool(int x, int y, t_infos *infos)
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
			infos->circles[i].id = i;
			infos->circles[i].pos.x = x;
			infos->circles[i].pos.y = y;
			infos->circles[i].force.x = 0;
			infos->circles[i].force.y = 0;
			infos->circles[i].is_locked = true;
			infos->circles[i].color = CIRCLE_COLOR_LOCKED;
			infos->circles[i].prev_pos = infos->circles[i].pos;
			draw_circle_at(&infos->img, x, y, infos->circles[i].color);
			break ;
		}
	}
}

static int detect_circle(int x, int y, t_infos *infos)
{
	for (int i = 0; i < NB_MAX_CIRCLES; i++)
	{
		if (infos->circles[i].id != -1)
		{
			t_vector2 center;
			center.x = infos->circles[i].pos.x + CIRCLE_RAY;
			center.y = infos->circles[i].pos.y + CIRCLE_RAY;
			if ((x - center.x) * (x - center.x) + (y - center.y) * (y - center.y)
				< CIRCLE_RAY * CIRCLE_RAY)
				return i;
		}
	}
	return -1;
}

static int	get_open_linked_id(t_circle *circle)
{
	for (int i = 0; i < NB_MAX_LINKS; i++)
	{
		if (circle->links[i].left_id == -1)
			return i;
	}
	return -1;
}

static bool	already_has_link(t_circle *circle, int id)
{
	for (int i = 0; i < NB_MAX_LINKS; i++)
	{
		if (circle->links[i].left_id == id)
			return false;
	}
	return true;
}

static void reset_circle_color(t_circle *circle)
{
	circle->is_locked ? (circle->color = CIRCLE_COLOR_LOCKED) : (circle->color = CIRCLE_COLOR_UNLOCKED);
}

static void fill_link_infos(t_link *link, int id_left, int id_right, t_infos *infos)
{
	link->left_id = id_left;
	link->right_id = id_right;
	link->length =
		sqrt((infos->circles[id_left].pos.x - infos->circles[id_right].pos.x)
		* (infos->circles[id_left].pos.x - infos->circles[id_right].pos.x)
		+ (infos->circles[id_left].pos.y - infos->circles[id_right].pos.y)
		* (infos->circles[id_left].pos.y - infos->circles[id_right].pos.y));
}

static void	create_link(int x, int y, int *nb_link_click, int id, t_infos *infos)
{
	static int	waiting_circle_id = -1;

	int	waiting_link_id;
	int	link_id;

	if (*nb_link_click == -1 || id == waiting_circle_id)
	{
		if (waiting_circle_id != -1)
			reset_circle_color(&infos->circles[waiting_circle_id]);
		waiting_circle_id = -1;
		*nb_link_click = 0;
		return ;
	}
	if (*nb_link_click == 0)
	{
		if (get_open_linked_id(&infos->circles[id]) != -1)
		{
			(*nb_link_click)++;
			waiting_circle_id = id;
			infos->circles[id].color = CIRCLE_COLOR_LINKING;
		}
		return ;
	}
	link_id = get_open_linked_id(&infos->circles[id]);
	if (link_id == -1)
		return ;
	waiting_link_id = get_open_linked_id(&infos->circles[waiting_circle_id]);
	if (!already_has_link(&infos->circles[id], waiting_circle_id))
		return ;
	if (!already_has_link(&infos->circles[waiting_circle_id], id))
		return ;
	fill_link_infos(&infos->circles[id].links[link_id], id, waiting_circle_id, infos);
	fill_link_infos(&infos->circles[waiting_circle_id].links[waiting_link_id], waiting_circle_id, id, infos);
	reset_circle_color(&infos->circles[waiting_circle_id]);
	*nb_link_click = 0;
	waiting_circle_id = -1;
}

static void click_left_mouse(int x, int y, t_infos *infos)
{
	int	id_circle = detect_circle(x, y, infos);
	if (id_circle != -1)
	{
		infos->circles[id_circle].is_locked = !infos->circles[id_circle].is_locked;
		if (infos->circles[id_circle].color != CIRCLE_COLOR_LINKING)
			reset_circle_color(&infos->circles[id_circle]);
	}
	else
		add_circle_to_pool(x, y, infos);
}


int	click_mouse(int key_code, int x, int y, t_infos *infos)
{
	static int	nb_link_click = 0;

	if (key_code == 1)
	{
		nb_link_click = -1;
		create_link(x, y, &nb_link_click, -1, infos);
		click_left_mouse(x, y, infos);
	}
	else if (key_code == 3)
	{
		int id = detect_circle(x, y, infos);
		if (id != -1)
			create_link(x, y, &nb_link_click, id, infos);
	}
	return (0);
}
