/* Shared variables */

#define CHUNK_SIZE 65000
#define PROTO_SIZE 1

#define META_TRANSMIT 1
#define META_REQUEST 2
#define DATA_TRANSMIT 3
#define DATA_REQUEST 4
#define NODE_TRANSMIT 5
#define NODE_REQUEST 6
#define KEY_TRANSMIT 7
#define KEY_REQUEST 8

struct workunit {
	void *chunk; /* pointer to chunk in memory */
	unsigned short int size;
};

#include <sys/types.h>

struct protocol {
	void *chunk;
	unsigned short int size;
	unsigned char type;
};

/* Prototypes */

char * os_sha1 (void *, unsigned short int);

unsigned char * os_sha1_file (int);

char * os_sha1_decode (unsigned char *);

char * os_meta_create (int, char[]);

void os_meta_dump (void *, unsigned short int);

int os_send (void *, unsigned short int, const char *);

void os_recv (void);

void os_store (void *, unsigned short int);

void os_pipeline_push (int, struct workunit *);

struct workunit * os_pipeline_pull (int);

void * os_proto_pkt_asm (unsigned char, unsigned short int, void *);

struct protocol * os_proto_pkt_dsm (void *, unsigned short int);
