#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <openssl/sha.h>
#include <sys/types.h>
#include <unistd.h>

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

char *
os_sha1_file(int fd)
{
	char *hash = malloc(41);
	char *input = malloc(10000);
	int length = 10000;
	SHA256_CTX context;
	unsigned char md[SHA256_DIGEST_LENGTH];

	lseek(fd, 0, SEEK_SET);

	SHA256_Init(&context);
	SHA256_Update(&context, (unsigned char*)input, length);
	SHA256_Final(md, &context);

	return hash;
}
