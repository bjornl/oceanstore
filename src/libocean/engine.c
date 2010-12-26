#include <stdio.h> /* printf */
#include <stdlib.h>
#include <wool.h>
#include <libocean.h>

TASK_1(int, engine, struct workunit *, wu)
{
	struct protocol *proto;

	printf("Engine called with arg: %p\n", (void *) wu);

	proto = os_proto_pkt_dsm(wu->chunk, wu->size);

	os_store(proto->chunk, proto->size);

	free(wu->chunk);
	free(wu);

	return 0;
}
