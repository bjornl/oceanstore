#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <openssl/sha.h>
#include <sys/types.h>
#include <unistd.h>

#define CHUNK_SIZE 64000

char *
os_sha1(char *chunk)
{
	unsigned char buf[20];
	char *hash = malloc(41);
	int i;

	SHA1((const unsigned char *) chunk, strlen(chunk), buf);

	for (i = 0 ; i < 20 ; i++)
		sprintf(hash, "%s%02x", hash, buf[i]);

	return hash;
}

unsigned char *
os_sha1_file(int fd)
{
	unsigned char *md = malloc(SHA_DIGEST_LENGTH);
	unsigned char *buf = malloc(CHUNK_SIZE);
	int len = 0;
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
	int i;

	for (i = 0 ; i < 20 ; i++)
		sprintf(hash, "%s%02x", hash, md[i]);

	return hash;
}
