#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> /* memset */

#include <libocean.h>

#define OSD_IPADDR "1.2.3.4" /* temporary */

int
main(int argc, char **argv)
{
	struct metadata *foo = malloc(sizeof(struct metadata));
	char *buf = NULL;
	char *hash;
	void *pkt;
	void *foop;
	int fd = 0;
	unsigned char *file_hash, *md;
	unsigned int metachunkctr = 1;

	printf("file to open: \"%s\"\n", argv[1]); 

	/* open input file */
	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		printf("failed to open file\n");
		return -1;
	}

	/* construct inital metadata */
	file_hash = os_sha1_file(fd);
	foop = foo;
	foo->chunk = os_meta_create(file_hash, argv[1]);
	foo->size = META_CHUNK_HEADER_SIZE;
	foo->next = NULL;

	buf = malloc(CHUNK_SIZE);
	memset(buf, 0, CHUNK_SIZE);

	lseek(fd, 0, SEEK_SET);
	int rc = 0;
	do {
		rc = read(fd, buf, CHUNK_SIZE);

		if (rc > 0) {
			hash = os_sha1(buf, rc);
			printf("sending payload with hash: %s\n", hash);
			pkt = os_proto_pkt_asm(DATA_TRANSMIT, rc, buf);
			os_send(pkt, rc + PROTO_SIZE, OSD_IPADDR);
			free(hash);
			usleep(20000);

			md = os_sha1_md(buf, rc);
			if ((foo->size + META_CHUNK_SEGMENT_SIZE) > CHUNK_SIZE) {
				printf("Metadata chunk is full, allocate next!\n");

				foo->next = malloc(sizeof(struct metadata));
				foo = foo->next;
				foo->chunk = os_meta_create(file_hash, argv[1]);
				foo->size = META_CHUNK_HEADER_SIZE;
				foo->next = NULL;
				os_meta_chunk(foo, metachunkctr, md, OSD_IPADDR);
			} else {
				os_meta_chunk(foo, metachunkctr, md, OSD_IPADDR);
			}
			if ((foo->size + META_CHUNK_SEGMENT_SIZE) > CHUNK_SIZE) {
				printf("Metadata chunk is full, allocate next!\n");

				foo->next = malloc(sizeof(struct metadata));
				foo = foo->next;
				foo->chunk = os_meta_create(file_hash, argv[1]);
				foo->size = META_CHUNK_HEADER_SIZE;
				foo->next = NULL;
				os_meta_chunk(foo, metachunkctr, md, "0.0.0.0");
			} else {
				os_meta_chunk(foo, metachunkctr, md, "0.0.0.0");
			}
			if ((foo->size + META_CHUNK_SEGMENT_SIZE) > CHUNK_SIZE) {
				printf("Metadata chunk is full, allocate next!\n");

				foo->next = malloc(sizeof(struct metadata));
				foo = foo->next;
				foo->chunk = os_meta_create(file_hash, argv[1]);
				foo->size = META_CHUNK_HEADER_SIZE;
				foo->next = NULL;
				os_meta_chunk(foo, metachunkctr, md, "0.0.0.0");
			} else {
				os_meta_chunk(foo, metachunkctr, md, "0.0.0.0");
			}
			metachunkctr++;
		}

		printf("read %d bytes from file\n", rc);
	} while (rc);

	close(fd);
	free(file_hash);

	foo = foop;
	while (foo != NULL) {
		os_meta_set_ctr(foo->chunk, metachunkctr-1);
		foo = foo->next;
	}

	foo = foop;
	while (foo != NULL) {
		os_meta_dump(foo->chunk, foo->size);
		foo = foo->next;
	}

	foo = foop;
	while (foo != NULL) {
		hash = os_sha1(foo->chunk, foo->size);
		printf("sending meta payload with hash: %s\n", hash);
		free(hash);
		pkt = os_proto_pkt_asm(META_TRANSMIT, foo->size, foo->chunk);
		os_send(pkt, foo->size + PROTO_SIZE, OSD_IPADDR);
		foo = foo->next;
		usleep(20000);
	}

	return 0;
}
