#include <stdio.h> /* printf */
#include <stdlib.h>
#include <string.h>
#include <libocean.h>

/*
 * Protocol packet assembler
 */
void *
os_proto_pkt_asm(unsigned char ptype, unsigned short size, void *chunk)
{
	void *pkt, *ptr;

	printf("Proto: %d (%d bytes)\n", ptype, (int) sizeof(ptype));

	pkt = malloc(size + PROTO_SIZE);
	ptr = pkt;

	memcpy(pkt, &ptype, PROTO_SIZE);

	pkt = pkt + PROTO_SIZE;

	memcpy(pkt, chunk, size);

	pkt = ptr;

	return pkt;
}

struct protocol *
os_proto_pkt_dsm(void *chunk, unsigned short int size)
{
	struct protocol *proto;

	proto = calloc(1, sizeof(struct protocol));

	memcpy(&proto->ptype, chunk, PROTO_SIZE);

	proto->size = size - PROTO_SIZE;
	proto->chunk = malloc(proto->size);

	memcpy(proto->chunk, chunk + PROTO_SIZE, proto->size);

	printf("Extracted:\n");
	printf("Ptype: %d\n", proto->ptype);
	printf("Size: %d\n", proto->size);
	printf("Chunk: \"%s\"\n", (char *) proto->chunk);

	return proto;
}
