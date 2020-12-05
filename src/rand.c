#include "rand.h"
#include <inttypes.h>

static uint8_t seed8 = 1;

void rand8_seed(int seed)
{
	seed8 = seed;
}

int rand8(void)
{
	seed8 = ((seed8 << 5) ^ 0x1f) + seed8;

	return seed8;
}
