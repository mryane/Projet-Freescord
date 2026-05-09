#include "server/Log.h"
#include "server/server.h"
#include "server/user.h"
#include "common/commonDef.h"

#include <bits/pthreadtypes.h>
#include <stdbool.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#define SRV_MAX_PENDING_CLIENTS 8
#define SRV_FROM_HANDLE() server* srv = (server*) handle;

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
} server;

srv_handle srv_alloc()
{
	return (server*) malloc(sizeof(server));
}

bool srv_init(srv_handle handle)
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

void srv_tick(srv_handle handle)
{
	SRV_FROM_HANDLE()

	user *newUser = user_accept(srv->socketHandle);

	if (newUser == NULL)
	{
		return;
	}

	pthread_t userThread;

	pthread_create(&userThread, NULL, handle_client, newUser);
	pthread_detach(userThread);
}

void srv_close(srv_handle handle)
{
	SRV_FROM_HANDLE()

	if (srv->socketHandle != -1)
	{
		close(srv->socketHandle);
	}
}

void *handle_client(void *clt)
{
	user_handle((user*) clt);
	return NULL;
}

int create_listening_sock(uint16_t port)
{
	int socketFd = socket(AF_INET, SOCK_STREAM, 0);

	if (socketFd == -1)
	{
		SRV_LOG_ERROR("La création du socket d'écoute a échoué.")
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
		SRV_LOG_ERROR("L'opération bind du socket a échoué.")
		return -1;
	}

	if (listen(socketFd, SRV_MAX_PENDING_CLIENTS))
	{
		SRV_LOG_ERROR("L'opération listen du socket a échoué.")
		return -1;
	}

	return socketFd;
}
