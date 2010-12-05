#include <stdio.h> /* printf */
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <libocean.h>

/* Desc: metadata chunk constructor
 * Returns: pointer to metadata chunk
 */
char *
os_meta_create(int fd, char file[256])
{
	void *meta = NULL;
	void *ptr;
	char *file_hash;
	u_int32_t generation = 7;
	int meta_size = 0, i;

	/* add generation number */
	meta_size = meta_size + sizeof(u_int32_t);
	meta = realloc(meta, meta_size);
	ptr = meta; /* save the start address */
	printf("reallocated to %d bytes\n", meta_size);
	memcpy(meta, &generation, sizeof(int32_t));

	/* add file key */
	file_hash = os_sha1_file(fd);
	printf("file hash: %s (%d bytes)\n", file_hash, (int) strlen(file_hash));
	meta_size = meta_size + (40 * sizeof(int8_t));
	meta = realloc(meta, meta_size);
	printf("reallocated to %d bytes\n", meta_size);
	meta = (char *) meta + sizeof(u_int32_t);
	memcpy(meta, file_hash, 40*sizeof(int8_t));

	/* add filepath */
	if (file[0] == '/') {
		for (i = 0 ; file[i] != '\0' ; i++)
			file[i] = file[i + 1];
		for (; i < 256 ; i++)
			file[i] = '\0';
	}
	meta = ptr;
	meta_size = meta_size + (256 * sizeof(int8_t));
	meta = realloc(meta, meta_size);
	printf("reallocated to %d bytes\n", meta_size);
	meta = (char *) meta + sizeof(u_int32_t) + (40 * sizeof(int8_t));
	memcpy(meta, file, 256*sizeof(int8_t));

	meta = ptr;
	os_meta_dump(meta);

	return meta;
}

void
os_meta_dump(void *meta)
{
	void *metap = meta;
	u_int32_t generation;
	char filekey[41], filename[257];

	printf("- dumping metadata block -\n");

	printf("Generation:\n");
	memcpy(&generation, metap, sizeof(u_int32_t));
	printf("\"%d\"\n", generation);

	metap = (char *) metap + sizeof(u_int32_t);

	printf("File key:\n");
	/* memcpy(&filekey, (char *) meta+sizeof(u_int32_t), 40*sizeof(int8_t)); */
	memcpy(&filekey, (char *) metap, 40*sizeof(int8_t));
	filekey[40] = '\0'; 
	printf("\"%s\"\n", filekey);

	metap = (char *) metap + (40 * sizeof(int8_t));

	printf("File name:\n");
	memcpy(&filename, (char *) metap, 256*sizeof(int8_t));
	printf("\"%s\"\n", filename);
}
