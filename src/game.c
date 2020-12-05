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

static void game_load(struct prog_ctx *ctx)
{
	struct game_ctx *game;
	int rc;

	ctx->state_ctx = calloc(sizeof(struct game_ctx), 1);
	game = ctx->state_ctx;

	game->font = font_load("data/font.bmp", 8, 8);

	rc = game_load_templates();
	if (rc) {
		ctx->quit = 1;
		return;
	}

	input_set_default_map(&ctx->input);

	game->player.tmp = &obj_template[OBJ_PLAYER];
	game->player.angle = 270;

	LIST_INIT(&game->obj_list);
	LIST_ADD(&game->obj_list, &game->player.elem);
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

	game_unload_templates();

	font_unload(game->font);

	free(ctx->state_ctx);
	ctx->state_ctx = NULL;
}

static void game_logic(struct prog_ctx *ctx)
{
	struct game_ctx *game = ctx->state_ctx;

	/* Handle player input. */
	if (ctx->input.key[KEY_BACK]) {
		ctx->input.key[KEY_BACK] = 0;
		ctx->quit = 1;
	}
	if (ctx->input.key[KEY_LEFT])
		game->player.angle = MOD(game->player.angle - 3, DEGREE_MAX);

	if (ctx->input.key[KEY_RIGHT])
		game->player.angle = MOD(game->player.angle + 3, DEGREE_MAX);
}

static void game_display(struct prog_ctx *ctx)
{
	struct game_ctx *game = ctx->state_ctx;
	struct list_head *elem;
	struct object *obj;
	int tile, cam_x, cam_y, x, y;

	display_background();

	cam_x = game->player.x + game->player.tmp->r - SCREEN_W / 2;
	cam_y = game->player.y + game->player.tmp->r - SCREEN_H / 2;

	LIST_FOREACH(&game->obj_list, elem) {
		obj = LIST_ITEM(elem, struct object, elem);

		x = obj->x + obj->tmp->r - (obj->tmp->tile_w / 2) - cam_x;
		y = obj->y + obj->tmp->r - (obj->tmp->tile_h / 2) - cam_y;

		tile = obj->angle / (DEGREE_MAX / obj->tmp->tiles);
		tile = tile % obj->tmp->tiles;

		image_display(obj->tmp->img, tile, x, y);
	}
}

struct program_state game_state = {
	.load = game_load,
	.unload = game_unload,
	.logic = game_logic,
	.display = game_display
};
