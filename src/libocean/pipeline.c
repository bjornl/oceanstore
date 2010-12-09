#include <stdio.h> /* printf */

#include <libocean.h>

void
os_pipeline_push(int id, struct workunit *wu)
{
	printf("pushed work unit %p on pipeline #%d\n", (void *) wu, id);
}

struct workunit *
os_pipeline_pull(int id)
{
	printf("pipeline pull\n");

	return NULL;
}
