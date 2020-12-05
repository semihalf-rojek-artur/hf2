#include "display.h"
#include <SDL/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include "image.h"

struct display_ctx {
	SDL_Surface *screen;
	SDL_Surface *screen_scaled;
};

static struct display_ctx ctx;

int display_init(void)
{
	int rc;

	rc = SDL_Init(SDL_INIT_VIDEO);
	if (rc) {
		fprintf(stderr, "SDL: Unable to initialize video backend: %d\n",
			rc);
		return rc;
	}

	SDL_WM_SetCaption("Homing Fever II (Meetup preview)", NULL);
	SDL_ShowCursor(SDL_DISABLE);

	ctx.screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, SCREEN_BPP,
				      SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (!ctx.screen) {
		fprintf(stderr, "SDL: Unable to set video mode %dx%d@%dbpp\n",
			SCREEN_W, SCREEN_H, SCREEN_BPP);
		return EINVAL;
	}

	printf("Video mode: %dx%d %dbpp %s %s\n", SCREEN_W, SCREEN_H,
	       SCREEN_BPP,
	       ctx.screen->flags & SDL_HWSURFACE ? "hardware surface" : "",
	       ctx.screen->flags & SDL_DOUBLEBUF ? "double buffered" : "");

	return 0;
}

void display_close(void)
{
	if (ctx.screen != ctx.screen_scaled)
		SDL_FreeSurface(ctx.screen);

	SDL_Quit();
}

void display_update(void)
{
	SDL_Flip(ctx.screen);
}

int display_frame_limit(void)
{
	static uint32_t prev_ticks, cur_ticks;
	float time;

	cur_ticks = SDL_GetTicks();
	time = cur_ticks - prev_ticks;

	if (time >= 1000.0 / SCREEN_FPS) {
		prev_ticks = cur_ticks;

		return 0;
	}

	return 1;
}

void *display_load_surface(const char *path)
{
	SDL_Surface *loaded, *optimized;
	uint32_t alpha;

	if (!path) {
		fprintf(stderr, "Missing bitmap file path\n");
		return NULL;
	}

	loaded = SDL_LoadBMP(path);
	if (!loaded) {
		fprintf(stderr, "Unable to load bitmap for %s\n", path);
		return NULL;
	}

	optimized = SDL_CreateRGBSurface(SDL_SWSURFACE, loaded->w, loaded->h,
					 SCREEN_BPP, 0, 0, 0, 0);
	if (!optimized) {
		fprintf(stderr, "Unable to create surface for %s\n", path);
		goto err_free;
	}

	SDL_BlitSurface(loaded, NULL, optimized, NULL);
	SDL_FreeSurface(loaded);

	alpha = SDL_MapRGB(optimized->format, 255, 0, 255);
	SDL_SetColorKey(optimized, SDL_SRCCOLORKEY, alpha);

	return optimized;

err_free:
	SDL_FreeSurface(loaded);

	return NULL;
}

void display_unload_surface(void *data)
{
	SDL_Surface *surface = data;

	SDL_FreeSurface(surface);
}

void display_blit_surface(void *data, struct clip *clip, int x, int y)
{
	SDL_Surface *surface = data;
	SDL_Rect r, c;

	r.x = x;
	r.y = y;

	if (clip) {
		c.x = clip->x;
		c.y = clip->y;
		c.w = clip->w;
		c.h = clip->h;
	}

	SDL_BlitSurface(surface, clip ? &c : NULL, ctx.screen, &r);
}
