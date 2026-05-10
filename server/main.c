#include "server/server.h"

#include <stdlib.h>

int main(int numArgs, const char **args)
{
    srv_handle myServer = srv_alloc();
    
    if (!srv_init(myServer))
    {
        return EXIT_FAILURE;
    }

    while (true)
    {
        srv_tick(myServer);
    }

    srv_close(myServer);
	return EXIT_SUCCESS;
}