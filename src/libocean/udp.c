#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <libocean.h>

#include <stdio.h> /* printf */

int
os_send(void *chunk, int size, const char *ipaddr)
{
	struct sockaddr_in saddr;
	int fd, len = 0;

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	memset(&saddr, 0, sizeof(struct sockaddr_in));

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(4321);
	inet_pton(AF_INET, ipaddr, &saddr.sin_addr);

	len = sendto(fd, chunk, size, 0, (const struct sockaddr *) &saddr, sizeof(saddr));

	close(fd);

	printf("sent %d bytes\n", len);

	return len;
}

void
os_recv(void)
{
	struct sockaddr_in saddr, saddrc;
	socklen_t slen = sizeof(saddrc);
	char buf[100000];
	int fd, len;

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	memset(&saddr, 0, sizeof(struct sockaddr_in));

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(4321);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(fd, (const struct sockaddr *) &saddr, sizeof(saddr));

	while(1) {
		len = recvfrom(fd, buf, 100000, 0, (struct sockaddr *) &saddrc, &slen);

		printf("received packet of %d bytes from %s:%d\n", len, inet_ntoa(saddrc.sin_addr), ntohs(saddrc.sin_port));

		os_store(buf, len);
	}
}
