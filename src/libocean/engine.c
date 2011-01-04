#include <stdio.h> /* printf */
#include <stdlib.h>
#include <wool.h>
#include <libocean.h>

TASK_1(int, engine, struct workunit *, wu)
{
	struct protocol *proto;

	printf("Engine called with arg: %p\n", (void *) wu);

	proto = os_proto_pkt_dsm(wu->chunk, wu->size);

	switch (proto->type) {
		case META_TRANSMIT:
			printf("Processing work unit of type Meta Transmit (%d)\n", META_TRANSMIT);
			break;
		case META_REQUEST:
			printf("Processing work unit of type Meta Request (%d)\n", META_REQUEST);
			break;
		case DATA_TRANSMIT:
			printf("Processing work unit of type Data Transmit (%d)\n", DATA_TRANSMIT);
			break;
		case DATA_REQUEST:
			printf("Processing work unit of type Data Request (%d)\n", DATA_REQUEST);
			break;
		case NODE_TRANSMIT:
			printf("Processing work unit of type Node Transmit (%d)\n", NODE_TRANSMIT);
			break;
		case NODE_REQUEST:
			printf("Processing work unit of type Node Request (%d)\n", NODE_REQUEST);
			break;
		case KEY_TRANSMIT:
			printf("Processing work unit of type Key Transmit (%d)\n", KEY_TRANSMIT);
			break;
		case KEY_REQUEST:
			printf("Processing work unit of type Key Request (%d)\n", KEY_REQUEST);
			break;
	}

	os_store(proto->chunk, proto->size);

	free(wu->chunk);
	free(wu);

	return 0;
}
