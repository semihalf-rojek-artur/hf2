#include "game.h"
#include "states.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include "font.h"
#include "image.h"
#include "list.h"
#include "math.h"
#include "object.h"
#include "rand.h"

struct game_ctx {
	struct object player;
	struct list_head obj_list;
	struct list_head trail_list;
	struct image *font;
	struct image *overlay;
	struct image *blip;
	long ticks;
	long cur_time;
	long best_time;
	int penalty;
};

static int game_load_templates(void)
{
	struct object_template *tmp = obj_template;
	int i;

	for (i = 0; i < OBJ_COUNT; ++i) {
		tmp[i].img = image_load(tmp[i].data,
					tmp[i].tiles,
					tmp[i].tile_w,
					tmp[i].tile_h,
					tmp[i].row_w);
		if (!tmp[i].img)
			return EINVAL;
	}

	return 0;
}

static void game_unload_templates(void)
{
	struct object_template *tmp = obj_template;
	int i;

	for (i = 0; i < OBJ_COUNT; ++i) {
		image_unload(tmp[i].img);
		tmp[i].img = NULL;
	}
}

static void game_reset(struct prog_ctx *ctx)
{
	struct game_ctx *game = ctx->state_ctx;

	game->ticks = 0;
	game->penalty = 0;
	game->player.remove = 0;
	game->player.prev_angle = game->player.angle;
}

static void game_load(struct prog_ctx *ctx)
{
	struct game_ctx *game;
	int rc;

	ctx->state_ctx = calloc(sizeof(struct game_ctx), 1);
	game = ctx->state_ctx;

	rand8_seed(time(NULL));

	game->font = font_load("data/font.bmp", 8, 8);
	game->overlay = image_load("data/overlay.bmp", 1,
				   SCREEN_W, SCREEN_H, 1);
	game->blip = image_load("data/blip.bmp", 3, 7, 7, 3);

	rc = game_load_templates();
	if (rc) {
		ctx->quit = 1;
		return;
	}

	input_set_default_map(&ctx->input);

	game->player.tmp = &obj_template[OBJ_PLAYER];
	game->player.angle = 192;

	LIST_INIT(&game->obj_list);
	LIST_ADD(&game->obj_list, &game->player.elem);

	LIST_INIT(&game->trail_list);

	game_reset(ctx);
}

static void game_unload(struct prog_ctx *ctx)
{
	struct game_ctx *game = ctx->state_ctx;
	struct list_head *elem, *next;
	struct object *obj;

	LIST_FOREACH_SAFE(&game->obj_list, elem, next) {
		obj = LIST_ITEM(elem, struct object, elem);

		LIST_REMOVE(elem);
		free(obj);
	}

	LIST_FOREACH_SAFE(&game->trail_list, elem, next) {
		obj = LIST_ITEM(elem, struct object, elem);

		LIST_REMOVE(elem);
		free(obj);
	}

	game_unload_templates();

	image_unload(game->blip);
	image_unload(game->overlay);
	font_unload(game->font);

	free(ctx->state_ctx);
	ctx->state_ctx = NULL;
}

static void game_spawn(struct list_head *list, enum object_type type,
		       struct object *target, int radius, int angle)
{
	struct object *obj;

	obj = calloc(sizeof(struct object), 1);
	obj->tmp = &obj_template[type];
	obj->ttl = TICKS_SEC * obj->tmp->ttl;

	obj->x = target->x + radius * cosine[angle];
	obj->y = target->y + radius * sine[angle];
	obj->angle = angle - (DEGREE_MAX / 2);

	LIST_ADD(list, &obj->elem);
}

static void game_spawn_random(struct list_head *list, enum object_type type,
			      struct object *target, int radius)
{
	int angle = MOD(rand8(), DEGREE_MAX);

	game_spawn(list, type, target, radius, angle);
}

static void game_spawn_batch(struct list_head *list, enum object_type type,
			     int count, struct object *target, int radius)
{
	int i;

	for (i = 0; i < count; ++i)
		game_spawn_random(list, type, target, radius);
}

static void game_logic(struct prog_ctx *ctx)
{
	struct game_ctx *game = ctx->state_ctx;
	struct list_head *elem, *elem2, *next;
	enum object_type obj_type, obj2_type;
	struct object *obj, *obj2;
	uint8_t angle_tmp;

	/* Clean up objects flagged in the previous frame. */
	LIST_FOREACH_SAFE(&game->trail_list, elem, next) {
		obj = LIST_ITEM(elem, struct object, elem);

		if (obj->remove)
			LIST_REMOVE(elem);
	}

	LIST_FOREACH_SAFE(&game->obj_list, elem, next) {
		obj = LIST_ITEM(elem, struct object, elem);

		if (obj->remove) {
			if (obj == &game->player)
				game_reset(ctx);
			else
				LIST_REMOVE(elem);
		}
	}

	/* Increment game time. */
	++game->ticks;
	game->cur_time = game->ticks;
	if (game->best_time < game->cur_time)
		game->best_time = game->cur_time;

	/* Handle player input. */
	if (ctx->input.key[KEY_BACK]) {
		ctx->input.key[KEY_BACK] = 0;
		ctx->quit = 1;
	}
	if (ctx->input.key[KEY_LEFT])
		game->player.angle = game->player.angle - 3;

	if (ctx->input.key[KEY_RIGHT])
		game->player.angle = game->player.angle + 3;

	game->player.vx = cosine[game->player.angle] * game->player.tmp->speed;
	game->player.vy = sine[game->player.angle] * game->player.tmp->speed;

	/* Spawn missiles at specified time intervals. */
	if (!(game->ticks % (TICKS_SEC * 60)))
		game_spawn_batch(&game->obj_list, OBJ_MISSILE_YELLOW, 10,
				 &game->player, 400);
	else if (!(game->ticks % (TICKS_SEC * 20)))
		game_spawn_random(&game->obj_list, OBJ_MISSILE_YELLOW,
				  &game->player, 400);
	else if (!(game->ticks % (TICKS_SEC * 10)))
		game_spawn_random(&game->obj_list, OBJ_MISSILE_BLUE,
				  &game->player, 400);
	else if (!(game->ticks % (TICKS_SEC * 5)))
		game_spawn_random(&game->obj_list, OBJ_MISSILE_RED,
				  &game->player, 400);

	/* Punish the player for constantly flying straight. */
	angle_tmp = game->player.angle - (game->player.prev_angle -
		    PLR_PENALTY_RANGE / 2);

	if (angle_tmp < PLR_PENALTY_RANGE) {
		if (++game->penalty >= TICKS_SEC * PLR_PENALTY_TIME) {
			game_spawn(&game->obj_list, OBJ_MISSILE_BLUE,
				   &game->player, 400, game->player.angle);

			game->penalty = 0;
			game->player.prev_angle = game->player.angle;
		}
	} else {
		game->penalty = 0;
		game->player.prev_angle = game->player.angle;
	}

	/* Move objects. */
	LIST_FOREACH(&game->obj_list, elem) {
		obj = LIST_ITEM(elem, struct object, elem);

		if (obj->tmp->logic)
			obj->tmp->logic(obj, &game->player);

		obj->x += obj->vx;
		obj->y += obj->vy;

		if (game->ticks % 3)
			continue;

		switch (object_get_type(obj)) {
		case OBJ_MISSILE_RED:
		case OBJ_MISSILE_BLUE:
		case OBJ_MISSILE_YELLOW:
			game_spawn_random(&game->trail_list, OBJ_TRAIL, obj, 0);
			break;

		default:
			break;
		}
	}

	LIST_FOREACH(&game->trail_list, elem) {
		obj = LIST_ITEM(elem, struct object, elem);

		if (obj->tmp->logic)
			obj->tmp->logic(obj, &game->player);
	}

	/* Perform collision detection. */
	LIST_FOREACH(&game->obj_list, elem) {
		obj = LIST_ITEM(elem, struct object, elem);
		obj_type = object_get_type(obj);

		LIST_FOREACH(elem, elem2) {
			if (elem2 == &game->obj_list)
				break;

			obj2 = LIST_ITEM(elem2, struct object, elem);
			obj2_type = object_get_type(obj2);

			if ((obj_type == OBJ_PLAYER ||
			    obj_type == OBJ_EXPLOSION) &&
			    (obj2_type == OBJ_PLAYER ||
			    obj2_type == OBJ_EXPLOSION))
				continue;

			if (object_collision(obj, obj2)) {
				obj2->remove = obj->remove = 1;

				game_spawn_batch(&game->obj_list, OBJ_EXPLOSION,
						 3, obj, 3);
				game_spawn_batch(&game->obj_list, OBJ_EXPLOSION,
						 3, obj2, 3);
			}
		}
	}
}

static void game_display(struct prog_ctx *ctx)
{
	struct game_ctx *game = ctx->state_ctx;
	struct list_head *elem;
	struct object *obj;
	char timer_txt[16];
	int radius, cam_x, cam_y, x, y;
	int angle;

	display_background(-FMOD(game->player.x, BACKGROUND_DIV),
			   -FMOD(game->player.y, BACKGROUND_DIV));

	cam_x = game->player.x + game->player.tmp->r - SCREEN_W / 2;
	cam_y = game->player.y + game->player.tmp->r - SCREEN_H / 2;

	/* Display visible objects. */
	LIST_FOREACH(&game->trail_list, elem) {
		obj = LIST_ITEM(elem, struct object, elem);

		radius = distance(obj->x, obj->y,
				   game->player.x, game->player.y);

		if (radius > RADAR_RADIUS)
			continue;

		x = obj->x + obj->tmp->r - (obj->tmp->tile_w / 2) - cam_x;
		y = obj->y + obj->tmp->r - (obj->tmp->tile_h / 2) - cam_y;

		angle = DEGREE_MAX / obj->tmp->tiles;

		image_display(obj->tmp->img, obj->angle / angle, x, y);
	}

	LIST_FOREACH(&game->obj_list, elem) {
		obj = LIST_ITEM(elem, struct object, elem);

		radius = distance(obj->x, obj->y,
				   game->player.x, game->player.y);

		if (radius > RADAR_RADIUS)
			continue;

		x = obj->x + obj->tmp->r - (obj->tmp->tile_w / 2) - cam_x;
		y = obj->y + obj->tmp->r - (obj->tmp->tile_h / 2) - cam_y;

		angle = DEGREE_MAX / obj->tmp->tiles;

		image_display(obj->tmp->img, obj->angle / angle, x, y);
	}

	image_display(game->overlay, 0, 0, 0);

	/* Display radar blips for objects outside of the visible area. */
	LIST_FOREACH(&game->obj_list, elem) {
		obj = LIST_ITEM(elem, struct object, elem);

		radius = distance(obj->x, obj->y,
				   game->player.x, game->player.y);

		if (radius <= RADAR_RADIUS)
			continue;

		angle = MOD(obj->radar_angle - (DEGREE_MAX / 2), DEGREE_MAX);
		x = (game->player.x + game->player.tmp->r + RADAR_RADIUS *
		     cosine[angle]) - (obj->tmp->tile_w / 2) - cam_x;
		y = game->player.y + game->player.tmp->r + RADAR_RADIUS *
		    sine[angle] - (obj->tmp->tile_h / 2) - cam_y;

		image_display(game->blip, object_get_type(obj) - 1, x, y);
	}

	/* Display game timer. */
	snprintf(timer_txt, 15, "%02u'%02u\"%02u",
		 (unsigned int)game->cur_time / SCREEN_FPS / TICKS_SEC,
		 (unsigned int)game->cur_time / SCREEN_FPS % TICKS_SEC,
		 (unsigned int)game->cur_time % SCREEN_FPS * 1000 / 600);
	font_display(timer_txt, 5, 20, game->font);

	if (game->cur_time == game->best_time)
		return;

	snprintf(timer_txt, 15, "%02u'%02u\"%02u",
		 (unsigned int)game->best_time / SCREEN_FPS / TICKS_SEC,
		 (unsigned int)game->best_time / SCREEN_FPS % TICKS_SEC,
		 (unsigned int)game->best_time % SCREEN_FPS * 1000 / 600);
	font_display(timer_txt, 5, 10, game->font);
}

struct program_state game_state = {
	.load = game_load,
	.unload = game_unload,
	.logic = game_logic,
	.display = game_display
};
