#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

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

	printf("sent %d bytes\n", len);

	return len;
}
