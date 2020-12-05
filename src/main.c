#include <stdlib.h>
#include <string.h>
#include "display.h"
#include "input.h"
#include "states.h"

int main(void)
{
	int rc;

	rc = display_init();
	if (rc)
		return rc;

	rc = input_init();
	if (rc)
		return rc;

	while (!state_handle());

	display_close();

	return 0;
}
