#include "object.h"
#include <math.h>
#include "game.h"
#include "math.h"

#define DATA_PATH	"data/"

static void missile_logic(struct object *obj, void *data)
{
}

struct object_template obj_template[OBJ_COUNT] = {
	{
		.data = DATA_PATH "player.bmp",
		.tiles = 32,
		.tile_w = 16,
		.tile_h = 16,
		.row_w = 8,
		.r = 6,
	},
	{
		.logic = missile_logic,
		.data = DATA_PATH "missile_red.bmp",
		.tiles = 16,
		.tile_w = 8,
		.tile_h = 8,
		.row_w = 8,
		.r = 3,
	},
	{
		.logic = missile_logic,
		.data = DATA_PATH "missile_blue.bmp",
		.tiles = 16,
		.tile_w = 8,
		.tile_h = 8,
		.row_w = 8,
		.r = 3,
	},
	{
		.logic = missile_logic,
		.data = DATA_PATH "missile_yellow.bmp",
		.tiles = 16,
		.tile_w = 8,
		.tile_h = 8,
		.row_w = 8,
		.r = 3,
	},
};

enum object_type object_get_type(struct object *obj)
{
	return obj->tmp - obj_template;
}
