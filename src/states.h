#ifndef _STATES_H_
#define _STATES_H_

#include "input.h"

struct prog_ctx {
	void *state_ctx;
	struct input input;
	int quit;
};

struct program_state {
	void (*load)(struct prog_ctx *ctx);
	void (*unload)(struct prog_ctx *ctx);
	void (*input)(struct prog_ctx *ctx);
	void (*logic)(struct prog_ctx *ctx);
	void (*display)(struct prog_ctx *ctx);
	struct program_state *state_change;
};

int state_handle(void);

#endif /* _STATES_H_ */
