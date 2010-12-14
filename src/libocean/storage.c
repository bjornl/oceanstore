#include <stdio.h> /* printf */
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <libocean.h>

void
os_store(void *chunk, unsigned short int size)
{
	char *hash;
	unsigned short int fd, len;

	hash = os_sha1(chunk, size);

	fd = open(hash, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
	len = write(fd, chunk, size);
	close(fd);

	printf("chunk %s of %d bytes written to disk\n", hash, len);

	free(hash);
}
