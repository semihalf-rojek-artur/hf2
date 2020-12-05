#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <inttypes.h>
#include "image.h"
#include "list.h"

#define PLR_PENALTY_RANGE	64
#define PLR_PENALTY_TIME	5

struct object;

enum object_type {
	OBJ_PLAYER,
	OBJ_MISSILE_RED,
	OBJ_MISSILE_BLUE,
	OBJ_MISSILE_YELLOW,
	OBJ_TRAIL,
	OBJ_EXPLOSION,
	OBJ_COUNT
};

struct object_template {
	void (*logic)(struct object *obj, void *data);
	struct image *img;
	const char *data;
	int tiles;
	int tile_w;
	int tile_h;
	int row_w;
	int r;
	int ttl;
	int step;
	float speed;
};

struct object {
	struct object_template *tmp;
	struct list_head elem;
	int remove;
	int ttl;
	float x;
	float y;
	float vx;
	float vy;
	uint8_t angle;
	uint8_t radar_angle;
	uint8_t prev_angle;
	int8_t turn_angle;
};

extern struct object_template obj_template[];

int object_collision(struct object *obj, struct object *obj2);
enum object_type object_get_type(struct object *obj);

#endif /* _OBJECT_H_ */
