#include <stdio.h> /* printf */
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <libocean.h>

/* Desc: metadata chunk constructor
 * Returns: pointer to metadata chunk
 */
char *
os_meta_create(int fd)
{
	void *meta = NULL;
	void *ptr;
	char *file_hash;
	u_int32_t generation = 7;
	u_int32_t test_gen = 0;
	char test_hash[41];
	int meta_size = 0;

	/* add generation number */
	meta_size = meta_size + sizeof(u_int32_t);
	meta = realloc(meta, meta_size);
	ptr = meta; /* save the start address */

	printf("reallocated to %d bytes\n", meta_size);

	memcpy(meta, &generation, sizeof(int32_t));

	memcpy(&test_gen, meta, sizeof(u_int32_t));

	printf("extracted gen from meta: \"%d\"\n", test_gen);

	file_hash = os_sha1_file(fd);

	printf("file hash: %s (%d bytes)\n", file_hash, (int) strlen(file_hash));

	printf("addr of meta before: %p\n", meta);

	meta_size = meta_size + (40 * sizeof(int8_t));
	meta = realloc(meta, meta_size);
	printf("reallocated to %d bytes\n", meta_size);

	meta = (char *) meta + sizeof(u_int32_t);

	printf("addr of meta after: %p\n", meta);


	memcpy(meta, file_hash, 40*sizeof(int8_t));

	meta = ptr;

	memcpy(&test_hash, (char *) meta+sizeof(u_int32_t), 40*sizeof(int8_t));
	test_hash[40] = '\0';
	printf("extracted hash from meta: \"%s\"\n", test_hash);

	return meta;
}
