#include <stdlib.h>
#include <libocean.h>

void *
os_proto_add(u_int8_t ptype, void *chunk)
{
	return NULL;
}

struct protocol *
os_proto_ext(void *chunk, unsigned short int size)
{
	struct protocol *proto;

	proto = calloc(1, sizeof(struct protocol));

	return proto;
}
