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

clt_handle clt_alloc()
{
	return malloc(sizeof(client));
}

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

bool clt_check_empty_line(const char *line)
{
	while (*line != '\n' && *line != '\0')
	{
		if (*line != ' ' && *line != '\t' && *line != '\r')
		{
			return false;
		}

		line++;
	}

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

bool clt_read_input(clt_handle handle, char *data, int n)
{
	CLT_FROM_HANDLE()

	if (buff_fgets(clt->inputBuffer, data, n) == NULL)
	{
		CLT_LOG("Erreur lors de la lecture de l'entrée.")
		return false;
	}

	if (lf_to_crlf(data) == NULL)
	{
		CLT_LOG("Erreur lors de la conversion de lf à crlf de l'entrée, probablement dû à une entrée trop longue.")
		return false;
	}

	return true;
}

bool clt_read_socket(clt_handle handle, char *data, int n)
{
	CLT_FROM_HANDLE()

	if (buff_fgets_crlf(clt->socketBuffer, data, n) == NULL)
	{
		CLT_LOG("Erreur lors de la lecture des données du serveur.")
	}

	if (crlf_to_lf(data) == NULL)
	{
		CLT_LOG("ATTENTION! Données invalides provenant du serveur!"
			" Il vous est fortement conseiller de ne plus vous y connecter!")
		return false;
	}

	clt_try_skip_null_char(clt);
	crlf_remove(data);

	return true;
}

bool clt_init(clt_handle handle)
{
	CLT_FROM_HANDLE()

	clt->clientId = -1;
	clt->socketHandle = clt_connect_to_server("192.168.1.180", SRV_PORT);

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

bool clt_receive_welcome_msg(clt_handle handle)
{
	CLT_FROM_HANDLE()

	char welcomeMsg[512];

	while (true)
	{
		if (! clt_read_socket(clt, welcomeMsg, sizeof(welcomeMsg)))
		{
			return false;
		}

		if (clt_check_empty_line(welcomeMsg))
		{
			return true;
		}

		printf("%s", welcomeMsg);
	}
}

bool clt_login(clt_handle handle)
{
	CLT_FROM_HANDLE()

	char nickname[24];
	char logCmd[64];
	char serverAnswer[64];
	int answerCode;

	while (true)
	{
		printf("Entrez votre surnom (pas plus de 16 caractères, le caractère ':' est interdit): ");
		fflush(stdout); /* On doit flush, car il n'y a pas de \n à la fin de notre printf */

		if (!clt_read_input(clt, nickname, 24))
		{
			printf("Erreur lors de la lecture du surnom, veuillez ressayer.\n");
			continue;
		}

		crlf_remove(nickname);

		snprintf(logCmd, 63, "nickname %s\r\n", nickname);
		send(clt->socketHandle, logCmd, strlen(logCmd) + 1, 0);

		if (!clt_read_socket(clt, serverAnswer, 64)
		 || sscanf(serverAnswer, "%d \n", &answerCode) != 1)
		{
			printf("Erreur lors de la lecture de la réponse du serveur, cette erreur est fatale.\n");
			exit(EXIT_FAILURE);
		}

		switch (answerCode)
		{
			case 0: printf("Vous êtes connecté avec le surnom %s.\n", nickname); return true;
			case 1: printf("Le surnom %s est déjà pris.\n", nickname); continue;
			case 2: printf("Le surnom %s est invalide.\n", nickname); continue;
			case 3:
				CLT_LOG("Réponse 3 reçu du serveur, cet erreur est fatale.") exit(EXIT_FAILURE);
			default:
				CLT_LOG("Réponse inconnue reçu du serveur (%d), cet erreur est fatale.", answerCode) exit(EXIT_FAILURE);
		}
	}
}

bool clt_handle_server_msg(clt_handle handle)
{
	CLT_FROM_HANDLE()
	char msg[512];

	if (!clt_read_socket(clt, msg, sizeof(msg)))
	{
		return false;
	}

	printf("%s", msg);
	return true;
}

bool clt_handle_input_msg(clt_handle handle)
{
	CLT_FROM_HANDLE()
	char msg[512];

	if (!clt_read_input(clt, msg, MAX_MSG_LENGTH))
	{
		return false;
	}

	return send(clt->socketHandle, msg, strlen(msg) + 1, 0) != -1;
}

bool clt_update(clt_handle handle)
{
	CLT_FROM_HANDLE()
	char msg[512];

	bool haveServerMsg = buff_ready(clt->socketBuffer);
	bool haveInputMsg = buff_ready(clt->inputBuffer);

	if (!haveServerMsg && !haveInputMsg)
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
		
		haveServerMsg = pollFd[1].revents != 0;
		haveInputMsg = pollFd[0].revents != 0;
	}

	if (haveServerMsg)
	{
		if (! clt_handle_server_msg(clt))
		{
			return false;
		}
	}

	if (haveInputMsg)
	{
		if (! clt_handle_input_msg(clt))
		{
			return false;
		}
	}

	return true;
}

void clt_run(clt_handle handle)
{
	CLT_FROM_HANDLE()

	if (!clt_receive_welcome_msg(clt))
	{
		return;
	}

	if (!clt_login(clt))
	{
		return;
	}

	while (true)
	{
		if (! clt_update(clt))
		{
			return;
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

int clt_connect_to_server(char *address, uint16_t port)
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
