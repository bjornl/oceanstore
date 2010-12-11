#include <stdio.h> /* printf */
#include <wool.h>

#include <libocean.h>

int CALL_engine (struct _Task *, struct workunit *);

void
os_pipeline_push(int id, struct workunit *wu)
{
	printf("pushed work unit %p on pipeline #%d\n", (void *) wu, id);

	ROOT_CALL(engine, wu);
}

struct workunit *
os_pipeline_pull(int id)
{
	printf("pipeline pull\n");

	return NULL;
}
