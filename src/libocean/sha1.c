#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <openssl/sha.h>

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
