#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#define SCREEN_W	320
#define SCREEN_H	240
#define SCREEN_BPP	32
#define SCREEN_FPS	60
#define BACKGROUND_DIV	16

#include "image.h"

int display_init(void);
void display_close(void);
void display_update(void);
int display_frame_limit(void);
void *display_load_surface(const char *path);
void display_unload_surface(void *data);
void display_blit_surface(void *data, struct clip *clip, int x, int y);
void display_background(void);

#endif /* _DISPLAY_H_ */
