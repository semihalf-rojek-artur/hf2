#ifndef _IMAGE_H_
#define _IMAGE_H_

struct clip {
	int x;
	int y;
	int w;
	int h;
};

struct image {
	void *data;
	struct clip *clip;
	int tiles;
};

struct image *image_load(const char *path, int tiles, int tile_w, int tile_h,
			 int row_w);
void image_unload(struct image *img);
void image_display(struct image *img, int tile, int x, int y);

#endif /* _IMAGE_H_ */
