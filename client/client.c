#include "client/Client.h"
#include "client/Log.h"
#include "CommonDef.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <poll.h>

#define CLT_FROM_HANDLE() Client* clt = (Client*) handle;

typedef struct
{
    int socketHandle;
    int clientId;
} Client;

/** se connecter au serveur TCP d'adresse donnée en argument sous forme de
 * chaîne de caractère et au port donné en argument
 * retourne le descripteur de fichier de la socket obtenue ou -1 en cas
 * d'erreur. */
int connect_serveur_tcp(char *adresse, uint16_t port);

bool CLTTryParseIPv4Address(const char *stringAddr, unsigned int *outAddr)
{
	unsigned int splitAddr[4];

	if (sscanf(stringAddr, "%u.%u.%u.%u", &splitAddr[0], &splitAddr[1], &splitAddr[2], &splitAddr[3]) != 4)
	{
		return false;
	}

	*outAddr = 0;
	for (int i = 0; i < 4; i++)
	{
		if (splitAddr[i] > 255)
		{
			return false;
		}

		*outAddr |= splitAddr[4 - i - 1] << (i << 3);
	}
	
	return true;
}

CLTHandle CLTAlloc()
{
	return malloc(sizeof(Client));
}

bool CLTInit(CLTHandle handle)
{
	CLT_FROM_HANDLE()

	clt->clientId = -1;
	clt->socketHandle = connect_serveur_tcp("127.0.0.1", SRV_PORT);

	if (clt->socketHandle == -1)
	{
		CLT_LOG("La création du client a echoué.")
		return false;
	}

	CLT_LOG("Connection au serveur établie !")
	return true;
}

bool CLTTick(CLTHandle handle)
{
	CLT_FROM_HANDLE()

	char sendMsg[32];
	char receiveMsg[32];

	strcpy(sendMsg, "Hello, I'm the Client !");
	write(clt->socketHandle, sendMsg, 32);
	read(clt->socketHandle, receiveMsg, 32);

	CLT_LOG("Message reçu du serveur: %s", receiveMsg)
}

void CLTClose(CLTHandle handle)
{
	CLT_FROM_HANDLE()

	CLT_LOG("Fermeture de la connection.")
	close(clt->socketHandle);
}

int connect_serveur_tcp(char *address, uint16_t port)
{
	int clientSocketHandle;
	unsigned int serverIp;
	struct sockaddr_in serverSocketAddr;

	if (!CLTTryParseIPv4Address(address, &serverIp))
	{
		CLT_LOG("Impossible de lire l'IP donnée.")
		return -1;
	}

	memset(&serverSocketAddr, 0, sizeof(struct sockaddr_in));

	serverSocketAddr.sin_addr.s_addr = htonl(serverIp);
	serverSocketAddr.sin_port = htons(port);
	serverSocketAddr.sin_family = AF_INET;

	clientSocketHandle = socket(AF_INET, SOCK_STREAM, 0);

	if (clientSocketHandle == -1)
	{
		CLT_LOG("Erreur lors de la création du socket client. Code: %d", errno)
		return -1;
	}

	if (connect(clientSocketHandle, (struct sockaddr*) &serverSocketAddr, sizeof(struct sockaddr_in)))
	{
		CLT_LOG("Erreur lors de la connection au serveur. Code: %d", errno)
		return -1;
	}

	return clientSocketHandle;
}
