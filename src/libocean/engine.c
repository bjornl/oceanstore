#include <stdio.h> /* printf */
#include <stdlib.h>
#include <wool.h>
#include <libocean.h>

TASK_1(int, engine, struct workunit *, wu)
{
	printf("Engine called with arg: %p\n", (void *) wu);

	os_store(wu->chunk, wu->size);

	free(wu->chunk);
	free(wu);

	return 0;
}
