#include "states.h"
#include <stddef.h>
#include "display.h"
#include "game.h"
#include "input.h"

static struct prog_ctx ctx;

static struct program_state *state = &(struct program_state){
	.state_change = &game_state,
};

int state_handle(void)
{
	static struct program_state *next;

	if (display_frame_limit())
		return 0;

	if (state->state_change) {
		next = state->state_change;

		if (state->unload)
			state->unload(&ctx);

		state->state_change = NULL;
		state = next;

		if (state->load)
			state->load(&ctx);
	}

	input_poll(&ctx.input);
//	if (state->input)
//		state->input(&ctx);
	if (state->logic)
		state->logic(&ctx);

	if (state->display)
		state->display(&ctx);

	display_update();

	return ctx.quit;
}
