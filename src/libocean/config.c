#include <stdio.h> /* printf */
#include <stdlib.h>
#include <libocean.h>

/**
 * Initiate configuration
 *
 * Lookup order:
 * 1. /etc/osd (system wide configuration)
 * 2. $HOME/.osd (user private configuration)
 * 3. $PWD (local working directory)
 */
struct config *
os_config_init(void)
{
	struct config *cfg;

	cfg = malloc(sizeof(struct config));

	return cfg;
}
