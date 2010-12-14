/* Shared variables */

struct workunit {
	void *chunk; /* pointer to chunk in memory */
	unsigned short int size;
};

#include <sys/types.h>

struct protocol {
	void *chunk;
	unsigned short int size;
	u_int8_t ptype;
};

/* Prototypes */

char * os_sha1 (void *, unsigned short int);

unsigned char * os_sha1_file (int);

char * os_sha1_decode (unsigned char *);

char * os_meta_create (int, char[]);

void os_meta_dump (void *);

int os_send (void *, unsigned short int, const char *);

void os_recv (void);

void os_store(void *, int);

void os_pipeline_push(int, struct workunit *);

struct workunit * os_pipeline_pull(int);
