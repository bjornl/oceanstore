#include <wool.h>
#include <libocean.h>

int
main(int argc, char **argv)
{
	wool_init(&argc, &argv);

	os_recv();

	return 0;
}
