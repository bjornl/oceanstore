#include <stdio.h> /* printf */
#include <stdlib.h>
#include <libocean.h>

/**
 * Load node configuration from file.
 */
struct nodes *
os_node_load(struct config *cfg)
{
	struct nodes *nds;

	nds = malloc(sizeof(struct nodes));

	printf("node config is located at \"%s\"\n", cfg->node);

	return nds;
}
