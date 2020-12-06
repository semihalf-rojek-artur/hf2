#include "object.h"
#include <math.h>
#include "game.h"
#include "math.h"

#define DATA_PATH	"data/"

static void trail_logic(struct object *obj, void *unused)
{
	(void)unused;

	if (obj->remove)
		return;

	if (--obj->ttl <= 0)
		obj->remove = 1;
}

static void explosion_logic(struct object *obj, void *unused)
{
	(void)unused;

	if (obj->remove)
		return;

	if (--obj->ttl <= 0)
		obj->remove = 1;
}

static void missile_logic(struct object *obj, void *data)
{
	struct object *target = data;
	float x, y;
	uint8_t target_angle;

	if (!data)
		goto velocity;

	x = (target->x + target->tmp->r) - (obj->x + obj->tmp->r);
	y = (target->y + target->tmp->r) - (obj->y + obj->tmp->r);

	target_angle = atan2(y, x) * DEGREE_MAX / 2 / PI;

	obj->radar_angle = target_angle;

	target_angle -= obj->angle;

	if (target_angle >= DEGREE_MAX / 2 &&
	    obj->turn_angle > -obj->tmp->step)
			--obj->turn_angle;
	else if (target_angle != obj->turn_angle &&
		   obj->turn_angle < obj->tmp->step)
			++obj->turn_angle;

	obj->angle += obj->turn_angle / 2;

velocity:
	obj->vx = cosine[obj->angle] * obj->tmp->speed;
	obj->vy = sine[obj->angle] * obj->tmp->speed;
}

struct object_template obj_template[OBJ_COUNT] = {
	{
		.data = DATA_PATH "player.bmp",
		.tiles = 32,
		.tile_w = 16,
		.tile_h = 16,
		.row_w = 8,
		.r = 6,
		.speed = 2.0,
	},
	{
		.logic = missile_logic,
		.data = DATA_PATH "missile_red.bmp",
		.tiles = 16,
		.tile_w = 8,
		.tile_h = 8,
		.row_w = 8,
		.r = 3,
		.step = 5,
		.speed = 2.5,
	},
	{
		.logic = missile_logic,
		.data = DATA_PATH "missile_blue.bmp",
		.tiles = 16,
		.tile_w = 8,
		.tile_h = 8,
		.row_w = 8,
		.r = 3,
		.step = 4,
		.speed = 2.5,
	},
	{
		.logic = missile_logic,
		.data = DATA_PATH "missile_yellow.bmp",
		.tiles = 16,
		.tile_w = 8,
		.tile_h = 8,
		.row_w = 8,
		.r = 3,
		.step = 3,
		.speed = 2.5,
	},
	{
		.logic = trail_logic,
		.data = DATA_PATH "trail.bmp",
		.tiles = 1,
		.tile_w = 3,
		.tile_h = 3,
		.row_w = 1,
		.r = 1,
		.ttl = 2,
	},
	{
		.logic = explosion_logic,
		.data = DATA_PATH "explosion.bmp",
		.tiles = 1,
		.tile_w = 5,
		.tile_h = 5,
		.row_w = 1,
		.r = 3,
		.ttl = 1,
	},
};

int object_collision(struct object *obj, struct object *obj2)
{
	int x, x2, y, y2, r, r2;

	r = obj->tmp->r;
	x = obj->x + r;
	y = obj->y + r;

	r2 = obj2->tmp->r;
	x2 = obj2->x + r2;
	y2 = obj2->y + r2;

	return distance_squared(x, y, x2, y2) <= (r * r + r2 * r2);
}

enum object_type object_get_type(struct object *obj)
{
	return obj->tmp - obj_template;
}
