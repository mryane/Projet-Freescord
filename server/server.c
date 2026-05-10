/* Ryane Menaï 12409318
 * Je déclare qu'il s'agit de mon propre travail.
 * Ce travail a été réalisé intégralement par un être humain. */

#include "server/Log.h"
#include "server/server.h"
#include "server/user.h"

#include "common/commonDef.h"
#include "common/list.h"

#include <bits/pthreadtypes.h>
#include <stdbool.h>
#include <stdlib.h>

#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#define SRV_MAX_PENDING_CLIENTS 8
#define SRV_FROM_HANDLE() server* srv = (server*) handle;

void* srv_repeat(srv_handle handle);

typedef struct
{
	int socketHandle;
	int pipeHandles[2];
	pthread_t repeatThread;
	pthread_mutex_t userListMutex;
	struct list *userList;
	_Atomic bool isClosing;
} server;

srv_handle srv_alloc()
{
	return (server*) malloc(sizeof(server));
}

bool srv_init(srv_handle handle)
{
	SRV_FROM_HANDLE()

	srv->socketHandle = srv_create_listening_sock(SRV_PORT);
	srv->userList = list_create();
	srv->isClosing = false;

	if (srv->socketHandle == -1 || pipe(srv->pipeHandles))
	{
		SRV_LOG("L'initialisation du serveur a échoué.")
		return false;
	}

	pthread_mutex_init(&srv->userListMutex, NULL);
	pthread_create(&srv->repeatThread, NULL, srv_repeat, srv);

	SRV_LOG("Initialisation terminé, écoute sur le port %u.", SRV_PORT)
	return true;
}

int srv_get_socket(srv_handle handle)
{
	SRV_FROM_HANDLE()
	return srv->socketHandle;
}

int srv_get_write_pipe(srv_handle handle)
{
	SRV_FROM_HANDLE()
	return srv->pipeHandles[1];
}

const char *srv_get_welcome_msg(srv_handle handle)
{
	SRV_FROM_HANDLE()

	return
		"Bienvenue sur le serveur Freescord!\r\n"
		"Envoyez des messages et recevez en d'autres personnes.\r\n"
		"Merci de respectez les règles de bienséances afin de garentir une bonne experience pour tout le monde\r\n"
		"\r\n";
}

bool srv_check_unused_nickname(srv_handle handle, const char *nickname)
{
	SRV_FROM_HANDLE()
	int count = 0;

	pthread_mutex_lock(&srv->userListMutex);
	{
		struct node *currentNode = list_get_node(srv->userList, 0);

		while (currentNode != NULL)
		{
			user *currentUser = (user *) currentNode->elt;

			count += (int) (strcmp(nickname, currentUser->nickname) == 0);

			if (count > 1)
			{
				break;
			}

			currentNode = list_get_next_node(currentNode);
		}
	}
	pthread_mutex_unlock(&srv->userListMutex);

	return count <= 1;
}

void srv_remove_user(srv_handle handle, struct user_t *u)
{
	SRV_FROM_HANDLE()

	if (srv->isClosing)
	{
		return;
	}

	pthread_mutex_lock(&srv->userListMutex);
	{
		if (list_contains(srv->userList, u))
		{
			list_remove_element(srv->userList, u);
		}
	}
	pthread_mutex_unlock(&srv->userListMutex);
}

void srv_tick(srv_handle handle)
{
	SRV_FROM_HANDLE()

	user *newUser = user_accept(srv);

	if (newUser == NULL)
	{
		return;
	}

	pthread_mutex_lock(&srv->userListMutex);
	{
		list_add(srv->userList, newUser);
	}
	pthread_mutex_unlock(&srv->userListMutex);

	pthread_t userThread;

	pthread_create(&userThread, NULL, (void* (*)(void*)) user_handle, newUser);
	pthread_detach(userThread);
}

void* srv_repeat(srv_handle handle)
{
	SRV_FROM_HANDLE()

	int i;
	int numReadedBytes;
	char readBuffer[1024];

	while (true)
	{
		numReadedBytes = read(srv->pipeHandles[0], readBuffer, sizeof(readBuffer));

		if (numReadedBytes == -1)
		{
			break;
		}

		pthread_mutex_lock(&srv->userListMutex);
		{
			struct node *currentNode = list_get_node(srv->userList, 0);

			while (currentNode != NULL)
			{
				send(((user*) currentNode->elt)->socketHandle, readBuffer, numReadedBytes, 0);
				currentNode = list_get_next_node(currentNode);
			}
		}
		pthread_mutex_unlock(&srv->userListMutex);
	}

	return NULL;
}

void srv_close(srv_handle handle)
{
	SRV_FROM_HANDLE()

	srv->isClosing = true;

	if (srv->socketHandle != -1)
	{
		close(srv->socketHandle);
	}

	close(srv->pipeHandles[0]);
	close(srv->pipeHandles[1]);

	pthread_join(srv->repeatThread, NULL);
	pthread_mutex_destroy(&srv->userListMutex);

	list_free(srv->userList, (void (*)(void*)) user_close);
}

int srv_create_listening_sock(uint16_t port)
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
