#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <openssl/sha.h>
#include <sys/types.h>
#include <unistd.h>
#include <libocean.h>

char *
os_sha1(void *chunk, unsigned short int size)
{
	unsigned char buf[20];
	char *hash = calloc(41, sizeof(char));
	unsigned char i;

	SHA1((const unsigned char *) chunk, size, buf);

	for (i = 0 ; i < 20 ; i++)
		sprintf(hash, "%s%02x", hash, buf[i]);

	return hash;
}

unsigned char *
os_sha1_md(void *chunk, unsigned short int size)
{
	unsigned char *md = malloc(SHA_DIGEST_LENGTH);

	SHA1((const unsigned char *) chunk, size, md);

	return md;
}

unsigned char *
os_sha1_file(int fd)
{
	unsigned char *md = malloc(SHA_DIGEST_LENGTH);
	unsigned char *buf = malloc(CHUNK_SIZE);
	unsigned short int len = 0;
	SHA_CTX context;

	lseek(fd, 0, SEEK_SET);

	SHA1_Init(&context);

	do {
		len = read(fd, buf, CHUNK_SIZE);
		if (len)
			SHA1_Update(&context, buf, len);
	} while (len);

	free(buf);
	SHA1_Final(md, &context);

	return md;
}

char *
os_sha1_decode(unsigned char *md)
{
	char *hash = calloc(41, sizeof(char));
	unsigned char i;

	for (i = 0 ; i < 20 ; i++)
		sprintf(hash, "%s%02x", hash, md[i]);

	return hash;
}
