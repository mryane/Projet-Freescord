#include "server/Server.h"

#include <stdlib.h>

SRVHandle gServer = SRV_NULL_HANDLE;

int main(int numArgs, const char **args)
{
    gServer = SRVAlloc();
    if (!SRVInit(gServer))
    {
        return EXIT_FAILURE;
    }

    while (true)
    {
        SRVTick(gServer);
    }

    SRVClose(gServer);
	return EXIT_SUCCESS;
}