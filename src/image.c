#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include "display.h"

struct image *image_load(const char *path, int tiles, int tile_w, int tile_h,
			 int row_w)
{
	struct image *img;
	int i;

	img = malloc(sizeof(struct image));
	if (!img) {
		fprintf(stderr, "Unable to allocate memory for image\n");
		return NULL;
	}

	img->data = display_load_surface(path);
	if (!img->data)
		goto err_free;

	img->clip = malloc(sizeof(struct clip) * tiles);
	if (!img->clip) {
		fprintf(stderr, "Unable to allocate memory for image clip\n");
		goto err_free;
	}

	/* Assume that all tiles have the same dimensions. */
	for (i = 0; i < tiles; ++i) {
		img->clip[i].x = i % row_w * tile_w;
		img->clip[i].y = i / row_w * tile_h;
		img->clip[i].w = tile_w;
		img->clip[i].h = tile_h;
	}

	img->tiles = tiles;

	return img;

err_free:
	free(img);

	return NULL;
}

void image_unload(struct image *img)
{
	if (!img)
		return;

	display_unload_surface(img->data);
	img->data = NULL;

	free(img->clip);
	img->clip = NULL;

	free(img);
}

void image_display(struct image *img, int tile, int x, int y)
{
	display_blit_surface(img->data, &img->clip[tile], x, y);
}
