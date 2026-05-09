#include "client/client.h"

#include <stdlib.h>

clt_handle gClient;

int main(int numArgs, const char **args)
{
    gClient = clt_alloc();
    if (!clt_init(gClient))
    {
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 3; i++)
    {
        clt_tick(gClient);
    }

    clt_close(gClient);
	return EXIT_SUCCESS;
}