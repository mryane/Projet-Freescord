#include "server/server.h"

#include <stdlib.h>

srv_handle gServer = SRV_NULL_HANDLE;

int main(int numArgs, const char **args)
{
    gServer = srv_alloc();
    
    if (!srv_init(gServer))
    {
        return EXIT_FAILURE;
    }

    while (true)
    {
        srv_tick(gServer);
    }

    srv_close(gServer);
	return EXIT_SUCCESS;
}