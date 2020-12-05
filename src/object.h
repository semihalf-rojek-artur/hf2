#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <inttypes.h>
#include "image.h"
#include "list.h"

struct object;

enum object_type {
	OBJ_PLAYER,
	OBJ_MISSILE_RED,
	OBJ_MISSILE_BLUE,
	OBJ_MISSILE_YELLOW,
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
};

struct object {
	struct object_template *tmp;
	struct list_head elem;
	float x;
	float y;
	float vx;
	float vy;
	int angle;
};

extern struct object_template obj_template[];

enum object_type object_get_type(struct object *obj);

#endif /* _OBJECT_H_ */
