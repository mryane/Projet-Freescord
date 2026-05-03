#include "server/user.h"
#include "server/Log.h"

#include <netinet/in.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>

user *user_accept(int serverSocketHandle)
{
    user *newUser;
    user tmpUser;

    tmpUser.addrLength = sizeof(tmpUser.addr);
    tmpUser.socketHandle = accept(
		serverSocketHandle,
		(struct sockaddr*) &tmpUser.addr,
		(socklen_t*) &tmpUser.addrLength);
	
	if (tmpUser.socketHandle == -1)
	{
		if (errno != EAGAIN)
		{
			SRV_LOG("Erreur lors de l'acceptation d'un nouveau client. Code: %d", errno)
		}
		
		return NULL;
	}

    newUser = (user*) malloc(sizeof(user));
    memcpy(newUser, &tmpUser, sizeof(user));

    return newUser;
}

void user_free(user *u)
{
    if (u->socketHandle != -1)
    {
        close(u->socketHandle);
    }

    free(u);
}