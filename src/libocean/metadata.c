#include <stdio.h> /* printf */
#include <stdlib.h> /* malloc */
#include <libocean.h>

/* Desc: metadata chunk constructor
 * Returns: pointer to metadata chunk
 */
char *
os_meta_create(int fd)
{
	char *meta, *file_hash;

	file_hash = os_sha1_file(fd);

	printf("file hash: %s\n", file_hash);

	meta = malloc(1000);

	return meta;
}
