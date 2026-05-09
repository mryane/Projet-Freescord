#include "client/client.h"
#include "client/Log.h"
#include "common/buffer.h"
#include "common/utils.h"
#include "common/commonDef.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <sys/poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>

#define CLT_FROM_HANDLE() client* clt = (client*) handle;

typedef struct
{
    int socketHandle;
    int clientId;
	buffer *socketBuffer;
	buffer *inputBuffer;
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
	clt->socketHandle = connect_server_tcp("192.168.1.180", SRV_PORT);

	if (clt->socketHandle == -1)
	{
		CLT_LOG("La création du client a echoué.")
		return false;
	}

	clt->socketBuffer = buff_create(clt->socketHandle, 1024);
	clt->inputBuffer = buff_create(STDIN_FILENO, 1024);

	CLT_LOG("Connection au serveur établie !")
	return true;
}

void clt_try_skip_null_char(clt_handle handle)
{
	CLT_FROM_HANDLE()

	if (buff_ready(clt->socketBuffer))
	{
		char nextChar = buff_getc(clt->socketBuffer);

		if (nextChar != '\0')
		{
			buff_ungetc(clt->socketBuffer, nextChar);
		}
	}
}

bool clt_read_input(clt_handle handle)
{
	CLT_FROM_HANDLE()
	char data[768];

	if (buff_fgets(clt->inputBuffer, data, 512) == NULL
	 || lf_to_crlf(data) == NULL)
	{
		return false;
	}

	return send(clt->socketHandle, data, strlen(data) + 1, 0) != -1;
}

bool clt_read_socket(clt_handle handle)
{
	CLT_FROM_HANDLE()
	char data[768];

	if (buff_fgets_crlf(clt->socketBuffer, data, 512) == NULL
	 || crlf_to_lf(data) == NULL)
	{
		return false;
	}

	clt_try_skip_null_char(clt);
	crlf_remove(data);

	printf("[CLIENT] Message reçu du serveur: %s", data);
	return true;
}

bool clt_tick(clt_handle handle)
{
	CLT_FROM_HANDLE()

	if (buff_ready(clt->socketBuffer))
	{
		return clt_read_socket(clt);
	}
	else if (buff_ready(clt->inputBuffer))
	{
		return clt_read_input(clt);
	}
	else
	{
		struct pollfd pollFd[2];
		
		pollFd[0].fd = STDIN_FILENO;
		pollFd[0].events = POLLIN;
		pollFd[0].revents = 0;

		pollFd[1].fd = clt->socketHandle;
		pollFd[1].events = POLLIN;
		pollFd[1].revents = 0;

		int pollResult = poll(pollFd, 2, -1);

		if (pollResult == -1)
		{
			CLT_LOG_ERROR("L'opération poll a échoué.")
			return false;
		}
		else
		{
			if (pollFd[0].revents != 0)
			{
				return clt_read_input(clt);
			}

			if (pollFd[1].revents != 0)
			{
				return clt_read_socket(clt);
			}

			__builtin_unreachable();
			return false;
		}
	}
}

void clt_close(clt_handle handle)
{
	CLT_FROM_HANDLE()
	CLT_LOG("Fermeture de la connection.")

	buff_free(clt->socketBuffer);
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
