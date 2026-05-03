#include "client/Client.h"

#include <stdlib.h>

CLTHandle gClient;

int main(int numArgs, const char **args)
{
    gClient = CLTAlloc();
    if (!CLTInit(gClient))
    {
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 3; i++)
    {
        CLTTick(gClient);
    }

    CLTClose(gClient);
	return EXIT_SUCCESS;
}