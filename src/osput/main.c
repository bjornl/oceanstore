#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h> /* memset */

#include <libocean.h>

#define CHUNK_SIZE 64000

int
main(int argc, char **argv)
{
	char *buf = NULL;
	char *meta;
	int fd = 0;

	printf("file to open: \"%s\"\n", argv[1]); 

	/* open input file */
	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		printf("failed to open file\n");
		return -1;
	}

	/* construct inital metadata */
	meta = os_meta_create(fd);

	/* some test code below */

	buf = malloc(CHUNK_SIZE);
	memset(buf, 0, CHUNK_SIZE);

	printf("data: \"%s\"\n", buf);

	int rc = 0;
	do {
		rc = read(fd, buf, CHUNK_SIZE);

		if (rc > 0) {
			int fdo = open("charlie.cnk", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
			int rc_out = write(fdo, buf, CHUNK_SIZE);
			close(fdo);
			printf("wrote chunk of %d bytes\n", rc_out);
		}

		printf("read %d bytes from file\n", rc);
	} while (rc);

	printf("data: \"%s\"\n", buf);

	close(fd);

	printf("%s\n", os_sha1("abc"));

	return 0;
}
