#include "input.h"
#include <SDL/SDL.h>

static SDL_Event event;
static int event_keys[SDLK_LAST];

int input_init(void)
{
	int rc;

	rc = SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	if (rc) {
		fprintf(stderr, "SDL: Unable to initialize joystick: %d\n", rc);
		return rc;
	}

	return 0;
}

void input_poll(struct input *input)
{
	int i;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			exit(0);
			break;
		case SDL_KEYDOWN:
			event_keys[event.key.keysym.sym] = 1;
			break;
		case SDL_KEYUP:
			event_keys[event.key.keysym.sym] = 0;
			break;
		default:
			break;
		}
	}

	for (i = 0; i < KEY_COUNT; ++i)
		input->key[i] = event_keys[input->map[i]];
}

void input_set_default_map(struct input *input)
{
	input->map[KEY_UP] = SDLK_UP;
	input->map[KEY_DOWN] = SDLK_DOWN;
	input->map[KEY_LEFT] = SDLK_LEFT;
	input->map[KEY_RIGHT] = SDLK_RIGHT;
	input->map[KEY_OK] = SDLK_RETURN;
	input->map[KEY_BACK] = SDLK_ESCAPE;
}
