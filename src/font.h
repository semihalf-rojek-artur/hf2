#ifndef _FONT_H_
#define _FONT_H_

#include "image.h"

struct image *font_load(const char *path, int glyph_w, int glyph_h);
void font_unload(struct image *font);
void font_display(const char *text, int x, int y, struct image *font);

#endif /* _FONT_H_ */
