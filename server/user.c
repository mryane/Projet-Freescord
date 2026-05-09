#include "server/user.h"
#include "server/Log.h"
#include "server/server.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

user *user_accept(int serverSocketHandle, int writePipeHandle)
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
		SRV_LOG_ERROR("Erreur lors de l'acceptation d'un nouveau client.")
		
		return NULL;
	}

    newUser = (user*) malloc(sizeof(user));
    memcpy(newUser, &tmpUser, sizeof(user));

    return newUser;
}

void user_handle(user *u)
{
    int newClientIp = ntohl(u->addr.ipv4Addr.sin_addr.s_addr);
	char receiveBuffer[64];

	SRV_LOG("Nouveau client ! Addresse: %d.%d.%d.%d",
		(newClientIp >> 24) & 0xFF,
		(newClientIp >> 16) & 0xFF,
		(newClientIp >> 8) & 0xFF,
		(newClientIp) & 0xFF)

	while (true)
	{
		int numReceivedBytes = recv(u->socketHandle, receiveBuffer, sizeof(receiveBuffer), 0);

		if (numReceivedBytes <= 0)
		{
			break;
		}

		

		send(u->socketHandle, receiveBuffer, numReceivedBytes, 0);
	}

	user_free(u);
	SRV_LOG("Connection avec le client fermé.")
}

void user_close(user *u)
{
    if (u->socketHandle != -1)
    {
        close(u->socketHandle);
    }
}

void user_free(user *u)
{
    user_close(u);
    free(u);
}