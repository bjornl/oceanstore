#include <stdio.h> /* printf */
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <openssl/sha.h>
#include <libocean.h>

/* Desc: metadata chunk constructor
 * Returns: pointer to metadata chunk
 */
char *
os_meta_create(unsigned char *file_hash, char *filep)
{
	void *meta = NULL;
	void *ptr;
	u_int32_t generation = 7, chunkctr = 0;
	int meta_size = 0, i;
	char file[256];

	/* add generation number */
	meta_size = meta_size + sizeof(u_int32_t);
	meta = realloc(meta, meta_size);
	ptr = meta; /* save the memory segment address */
	printf("reallocated to %d bytes\n", meta_size);
	memcpy(meta, &generation, sizeof(int32_t));

	/* add file key */
	meta_size = meta_size + (SHA_DIGEST_LENGTH * sizeof(unsigned char));
	meta = realloc(meta, meta_size);
	ptr = meta; /* save the memory segment address */
	printf("reallocated to %d bytes\n", meta_size);
	meta = (char *) meta + sizeof(u_int32_t);
	memcpy(meta, file_hash, SHA_DIGEST_LENGTH * sizeof(unsigned char));

	memset(&file, 0, 256);
	memcpy(&file, filep, strlen(filep));

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
	ptr = meta; /* save the memory segment address */
	printf("reallocated to %d bytes\n", meta_size);
	meta = (char *) meta + sizeof(u_int32_t) + (SHA_DIGEST_LENGTH * sizeof(unsigned char));
	memcpy(meta, file, 256*sizeof(int8_t));

	meta = ptr;
	meta_size = meta_size + sizeof(u_int32_t);
	meta = realloc(meta, meta_size);
	ptr = meta; /* save the memory segment address */
	printf("reallocated to %d bytes\n", meta_size);
	meta = (char *) meta + sizeof(u_int32_t) + (SHA_DIGEST_LENGTH * sizeof(unsigned char)) + (256*sizeof(int8_t));
	memcpy(meta, &chunkctr, sizeof(u_int32_t));

	meta = ptr;
	os_meta_dump(meta, meta_size);

	return meta;
}

void
os_meta_dump(void *meta, unsigned short int size)
{
	void *metap = meta;
	u_int32_t generation, chunkctr, chunkid;
	char filename[257];
	unsigned char filekey[SHA_DIGEST_LENGTH];
	unsigned char chunkkey[SHA_DIGEST_LENGTH];
	char chunkip[META_CHUNK_IP_FIELD_SIZE];
	unsigned short int chunksegs, i;

	printf("- dumping metadata block of %d bytes -\n", size);

	memcpy(&generation, metap, sizeof(u_int32_t));
	printf("Generation: \"%d\"\n", generation);

	metap = (char *) metap + sizeof(u_int32_t);

	memcpy(&filekey, (unsigned char *) metap, SHA_DIGEST_LENGTH * sizeof(unsigned char));
	printf("File key: \"%s\"\n", os_sha1_decode(filekey));

	metap = (char *) metap + (SHA_DIGEST_LENGTH * sizeof(unsigned char));

	memcpy(&filename, (char *) metap, 256*sizeof(int8_t));
	printf("File name: \"%s\"\n", filename);

	metap = (char *) metap + (256*sizeof(int8_t));

	memcpy(&chunkctr, metap, sizeof(u_int32_t));
	printf("Chunkctr: \"%d\"\n", chunkctr);

	metap = (char *) metap + sizeof(u_int32_t);

	if (size > META_CHUNK_HEADER_SIZE) {

		chunksegs = (size - META_CHUNK_HEADER_SIZE) / META_CHUNK_SEGMENT_SIZE;
		printf("num chunksegs in this block: \"%d\"\n", chunksegs);

		for (i=1 ; i <= chunksegs ; i++) {

			memcpy(&chunkid, metap, sizeof(u_int32_t));
			printf("#%d chunkid: \"%d\"\n", i, chunkid);

			metap = (char *) metap + sizeof(u_int32_t);

			memcpy(&chunkkey, (unsigned char *) metap, SHA_DIGEST_LENGTH);
			printf("#%d chunkkey: \"%s\"\n", i, os_sha1_decode(chunkkey));

			metap = (char *) metap + SHA_DIGEST_LENGTH;

			memcpy(&chunkip, (unsigned char *) metap, META_CHUNK_IP_FIELD_SIZE);
			printf("#%d chunkip: \"%s\"\n", i, chunkip);

			metap = (char *) metap + META_CHUNK_IP_FIELD_SIZE;
		}
	}
}

void
os_meta_chunk(struct metadata *meta, u_int32_t chunkid, unsigned char *md, char *ip)
{
	char addr[META_CHUNK_IP_FIELD_SIZE];

	printf("- adding chunk to metadata block of %d bytes -\n", meta->size);

	meta->size += sizeof(u_int32_t);
	meta->chunk = realloc(meta->chunk, meta->size);
	printf("reallocated to %d bytes\n", meta->size);

	memcpy(meta->chunk+(meta->size-sizeof(u_int32_t)), &chunkid, sizeof(u_int32_t));

	meta->size += SHA_DIGEST_LENGTH;
	meta->chunk = realloc(meta->chunk, meta->size);
	printf("reallocated to %d bytes\n", meta->size);

	memcpy(meta->chunk+(meta->size-SHA_DIGEST_LENGTH), md, SHA_DIGEST_LENGTH);

	meta->size += META_CHUNK_IP_FIELD_SIZE;
	meta->chunk = realloc(meta->chunk, meta->size);
	printf("reallocated to %d bytes\n", meta->size);

	memset(&addr, 0, META_CHUNK_IP_FIELD_SIZE);
	memcpy(&addr, ip, strlen(ip));
	memcpy(meta->chunk+(meta->size-META_CHUNK_IP_FIELD_SIZE), addr, META_CHUNK_IP_FIELD_SIZE);
}

void
os_meta_set_ctr(void *meta, u_int32_t chunkctr)
{
	void *metap = meta;

	printf("- update metadata block with chunkctr: %d\n", chunkctr);

	metap = (char *) metap + sizeof(u_int32_t);
	metap = (char *) metap + (SHA_DIGEST_LENGTH * sizeof(unsigned char));
	metap = (char *) metap + (256*sizeof(int8_t));

	memcpy(metap, &chunkctr, sizeof(u_int32_t));
}

/*
 * Support function, returns the chunkctr of a meta chunk.
 */
u_int32_t
os_meta_get_ctr(void *meta)
{
	void *metap = meta;
	u_int32_t chunkctr;

	metap = (char *) metap + sizeof(u_int32_t);
	metap = (char *) metap + (SHA_DIGEST_LENGTH * sizeof(unsigned char));
	metap = (char *) metap + (256*sizeof(int8_t));

	memcpy(&chunkctr, metap, sizeof(u_int32_t));

	return chunkctr;
}
