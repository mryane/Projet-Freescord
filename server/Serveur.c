#include "server/Log.h"
#include "server/Server.h"
#include "server/user.h"
#include "CommonDef.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>

#define SRV_MAX_PENDING_CLIENTS 8
#define SRV_FROM_HANDLE() Server* srv = (Server*) handle;

/** Gérer toutes les communications avec le client renseigné dans
 * user, qui doit être l'adresse d'une struct user */
void *handle_client(void *user);
/** Créer et configurer une socket d'écoute sur le port donné en argument
 * retourne le descripteur de cette socket, ou -1 en cas d'erreur */
int create_listening_sock(uint16_t port);

typedef struct
{
	int socketHandle;
	int numClients;
} Server;

SRVHandle SRVAlloc()
{
	return (Server*) malloc(sizeof(Server));
}

bool SRVInit(SRVHandle handle)
{
	SRV_FROM_HANDLE()

	srv->socketHandle = create_listening_sock(SRV_PORT);
	srv->numClients = 0;

	if (srv->socketHandle == -1)
	{
		SRV_LOG("L'initialisation du serveur a échoué.")
	}
	else
	{
		SRV_LOG("Initialisation terminé, écoute sur le port %u.", SRV_PORT)
	}

	return srv->socketHandle != -1;
}

void SRVTick(SRVHandle handle)
{
	SRV_FROM_HANDLE()

	user *newUser = user_accept(srv->socketHandle);

	if (newUser == NULL)
	{
		return;
	}

	char *newClientIp = (char*) &newUser->addr.ipv4Addr.sin_addr.s_addr;
	char receiveBuffer[64];

	SRV_LOG("Nouveau client ! Addresse: %d.%d.%d.%d",
		newClientIp[0],
		newClientIp[1],
		newClientIp[2],
		newClientIp[3])

	while (true)
	{
		int numReceivedBytes = read(newUser->socketHandle, receiveBuffer, sizeof(receiveBuffer));

		if (numReceivedBytes <= 0)
		{
			break;
		}

		write(newUser->socketHandle, receiveBuffer, numReceivedBytes);
	}

	user_free(newUser);
	SRV_LOG("Connection avec le client fermé.")
}

void SRVClose(SRVHandle handle)
{
	SRV_FROM_HANDLE()

	if (srv->socketHandle != -1)
	{
		close(srv->socketHandle);
	}
}

void *handle_client(void *clt)
{
	return clt;
}

int create_listening_sock(uint16_t port)
{
	int socketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

	if (socketFd == -1)
	{
		SRV_LOG("La création du socket d'écoute a échoué. Code: %d", errno)
		return -1;
	}

	struct sockaddr_in socketAddress =
	{
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr = 0
	};

	if (bind(socketFd, (struct sockaddr*) &socketAddress, sizeof(socketAddress)))
	{
		SRV_LOG("L'opération bind du socket a échoué.")
		return -1;
	}

	if (listen(socketFd, SRV_MAX_PENDING_CLIENTS))
	{
		SRV_LOG("L'opération listen du socket a échoué.")
		return -1;
	}

	return socketFd;
}
