#include "font.h"
#include <stddef.h>
#include <string.h>

#define FONT_ROW_W	16
#define FONT_GLYPHS	128

struct image *font_load(const char *path, int glyph_w, int glyph_h)
{
	return image_load(path, FONT_GLYPHS, glyph_w, glyph_h, FONT_ROW_W);
}

void font_unload(struct image *font)
{
	image_unload(font);
}

void font_display(const char *text, int x, int y, struct image *font)
{
	int pos_x, pos_y;
	unsigned int i;
	size_t len;

	len = strlen(text);
	pos_x = x;
	pos_y = y;

	for (i = 0; i < len; ++i) {
		if (text[i] == '\n') {
			pos_x = x;
			pos_y += font->clip[i].h;

			continue;
		}

		image_display(font, text[i], pos_x, pos_y);
		pos_x += font->clip[i].w;
	}
}
