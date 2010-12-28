#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> /* memset */

#include <libocean.h>

int
main(int argc, char **argv)
{
	char *buf = NULL;
	char *meta, *hash;
	void *pkt;
	int fd = 0, len;

	printf("file to open: \"%s\"\n", argv[1]); 

	/* open input file */
	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		printf("failed to open file\n");
		return -1;
	}

	/* construct inital metadata */
	meta = os_meta_create(fd, argv[1]);

	/* some test code below */

	buf = malloc(CHUNK_SIZE);
	memset(buf, 0, CHUNK_SIZE);

	printf("data: \"%s\"\n", buf);

	lseek(fd, 0, SEEK_SET);
	int rc = 0;
	do {
		rc = read(fd, buf, CHUNK_SIZE);

		if (rc > 0) {
			hash = os_sha1(buf, rc);
			printf("sending payload with hash: %s\n", hash);
			pkt = os_proto_pkt_asm(3, rc, buf);
			/* len = os_send(buf, rc, "1.2.3.4"); */
			len = os_send(pkt, rc + PROTO_SIZE, "1.2.3.4");
			free(hash);
			usleep(10000);

			/*
			int fdo = open("charlie.cnk", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
			int rc_out = write(fdo, buf, CHUNK_SIZE);
			close(fdo);
			printf("wrote chunk of %d bytes\n", rc_out);
			*/
		}

		printf("read %d bytes from file\n", rc);
	} while (rc);

	printf("data: \"%s\"\n", buf);

	close(fd);

	os_meta_dump(meta, 284);

	os_meta_chunk(meta, 284);

	printf("%s\n", os_sha1("abc", 3));

	return 0;
}
