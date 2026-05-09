#include "client/client.h"
#include "client/Log.h"
#include "common/commonDef.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define CLT_FROM_HANDLE() client* clt = (client*) handle;

typedef struct
{
    int socketHandle;
    int clientId;
} client;

/** se connecter au serveur TCP d'adresse donnée en argument sous forme de
 * chaîne de caractère et au port donné en argument
 * retourne le descripteur de fichier de la socket obtenue ou -1 en cas
 * d'erreur. */
int connect_server_tcp(char *adresse, uint16_t port);

bool clt_try_parse_ipv4_address(const char *stringAddr, unsigned int *outAddr)
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

clt_handle clt_alloc()
{
	return malloc(sizeof(client));
}

bool clt_init(clt_handle handle)
{
	CLT_FROM_HANDLE()

	clt->clientId = -1;
	clt->socketHandle = connect_server_tcp("127.0.0.1", SRV_PORT);

	if (clt->socketHandle == -1)
	{
		CLT_LOG("La création du client a echoué.")
		return false;
	}

	CLT_LOG("Connection au serveur établie !")
	return true;
}

void clt_tick(clt_handle handle)
{
	CLT_FROM_HANDLE()

	char sendMsg[32];
	char receiveMsg[32];

	strcpy(sendMsg, "Hello, I'm the Client !");
	write(clt->socketHandle, sendMsg, 32);
	read(clt->socketHandle, receiveMsg, 32);

	CLT_LOG("Message reçu du serveur: %s", receiveMsg)
}

void clt_close(clt_handle handle)
{
	CLT_FROM_HANDLE()

	CLT_LOG("Fermeture de la connection.")
	close(clt->socketHandle);
}

int connect_server_tcp(char *address, uint16_t port)
{
	int clientSocketHandle;
	unsigned int serverIp;
	struct sockaddr_in serverSocketAddr;

	if (!clt_try_parse_ipv4_address(address, &serverIp))
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
		CLT_LOG_ERROR("Erreur lors de la création du socket client.")
		return -1;
	}

	if (connect(clientSocketHandle, (struct sockaddr*) &serverSocketAddr, sizeof(struct sockaddr_in)))
	{
		CLT_LOG_ERROR("Erreur lors de la connection au serveur")
		return -1;
	}

	return clientSocketHandle;
}
