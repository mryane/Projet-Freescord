#include "client/client.h"

#include <stdlib.h>

clt_handle gClient;

int main(int numArgs, const char **args)
{
    gClient = clt_alloc();
    if (! clt_init(gClient))
    {
        return EXIT_FAILURE;
    }

    while (true)
    {
        if (!clt_tick(gClient))
        {
            break;
        }
    }

    clt_close(gClient);
	return EXIT_SUCCESS;
}