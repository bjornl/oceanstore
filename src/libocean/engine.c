#include <stdio.h> /* printf */
#include <wool.h>
#include <libocean.h>

TASK_1(int, engine, struct workunit *, wu)
{
	printf("Engine called with arg: %p\n", (void *) wu);

	os_store(wu->chunk, wu->size);

	return 0;
}
