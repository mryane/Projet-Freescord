/* Ryane Menaï 12409318
 * Je déclare qu'il s'agit de mon propre travail.
 * Ce travail a été réalisé intégralement par un être humain. */

 #include "client/client.h"

#include <stdlib.h>

int main(int numArgs, const char **args)
{
    clt_handle myClient = clt_alloc();

    if (! clt_init(myClient))
    {
        return EXIT_FAILURE;
    }

    clt_run(myClient);
    clt_close(myClient);

	return EXIT_SUCCESS;
}