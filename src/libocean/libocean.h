/* Prototypes */

char * os_sha1 (char *);

/* char * os_sha1_file (int); */
unsigned char * os_sha1_file (int);

char * os_sha1_decode (unsigned char *);

char * os_meta_create (int, char[]);

void os_meta_dump (void *);

int os_send (void *, int, const char *);
