/* Ryane Menaï 12409318
 * Je déclare qu'il s'agit de mon propre travail.
 * Ce travail a été réalisé intégralement par un être humain. */

#include "server/user.h"
#include "common/commonDef.h"
#include "server/Log.h"
#include "server/server.h"

#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

user *user_accept(srv_handle serverHandle)
{
    user *newUser;
    user tmpUser;

    tmpUser.addrLength = sizeof(tmpUser.addr);
	tmpUser.serverHandle = serverHandle;
    tmpUser.socketHandle = accept(
		srv_get_socket(serverHandle),
		(struct sockaddr*) &tmpUser.addr,
		(socklen_t*) &tmpUser.addrLength);
	
	if (tmpUser.socketHandle == -1)
	{
		SRV_LOG_ERROR("Erreur lors de l'acceptation d'un nouveau client.")
		
		return NULL;
	}

    newUser = (user*) malloc(sizeof(user));
    memcpy(newUser, &tmpUser, sizeof(user));
	memset(newUser->nickname, 0, sizeof(newUser->nickname));

    return newUser;
}

void user_send_welcome_msg(user *u)
{
	const char *welcomeMsg = srv_get_welcome_msg(u->serverHandle);

	send(u->socketHandle, welcomeMsg, strlen(welcomeMsg) + 1, 0);
}

bool user_check_login_cmd(const char *loginCmd)
{
	const char *loginCmdStart = "nickname ";

	while (*loginCmdStart != '\0')
	{
		if (*loginCmdStart != *loginCmd)
		{
			return false;
		}

		loginCmd++;
		loginCmdStart++;
	}

	return true;
}

bool user_check_nickname(user *u)
{
	const char *ptr = u->nickname;
	int i;

	for (i = 0; i < 24; i++)
	{
		if (*ptr == ':')
		{
			return false;
		}

		if (*ptr == '\0')
		{
			break;
		}

		ptr++;
	}

	return i < 16;
}

void user_send_login_answer(user *u, int answerCode)
{
	char formatedAnswerString[64];

	snprintf(formatedAnswerString, 63, "%d \r\n", answerCode);
	send(u->socketHandle, formatedAnswerString, strlen(formatedAnswerString) + 1, 0);
}

bool user_handle_login(user *u)
{
	char receiveBuffer[64];
	int numReadedBytes;

	while (true)
	{
		numReadedBytes = recv(u->socketHandle, receiveBuffer, sizeof(receiveBuffer), 0);

		if (numReadedBytes < 1 || numReadedBytes > 512)
		{
			return false;
		}

		if (!user_check_login_cmd(receiveBuffer))
		{
			user_send_login_answer(u, 3);
		}
		else if (sscanf(receiveBuffer, "nickname %s\r\n", u->nickname) != 1
		 || !user_check_nickname(u))
		{
			user_send_login_answer(u, 2);
		}
		else if (!srv_check_unused_nickname(u->serverHandle, u->nickname))
		{
			user_send_login_answer(u, 1);
		}
		else
		{
			user_send_login_answer(u, 0);
			break;
		}
	}

	const unsigned char *ip = (const unsigned char *) &u->addr.ipv4Addr.sin_addr.s_addr;

	SRV_LOG("%u.%u.%u.%u connecté sous le surnom \"%s\"",
		ip[0], ip[1], ip[2], ip[3], u->nickname)
	return true;
}

void user_handle_msg(user *u)
{
	char receiveBuffer[512];
	char sendBuffer[512];
	int numReceivedBytes;

	while (true)
	{
		numReceivedBytes = recv(u->socketHandle, receiveBuffer, sizeof(receiveBuffer), 0);

		if (numReceivedBytes <= 0)
		{
			break;
		}
		else if (numReceivedBytes > 512)
		{
			SRV_LOG("L'utilisateur \"%s\" a envoyé trop de données, arrêt de la connection.", u->nickname)
			break;
		}

		receiveBuffer[MAX_MSG_LENGTH] = '\0';
		snprintf(sendBuffer, 511, "%s: %s", u->nickname, receiveBuffer);

		SRV_LOG("Message reçu de \"%s\", recopie en cours...", u->nickname)

		write(srv_get_write_pipe(u->serverHandle), sendBuffer, strlen(sendBuffer) + 1);
	}
}

void user_handle_logoff(user *u)
{
	unsigned int ip = ntohl(u->addr.ipv4Addr.sin_addr.s_addr);

	srv_remove_user(u->serverHandle, u);
	user_free(u);

	SRV_LOG("Connection avec le client d'addresse %u.%u.%u.%u fermé.",
		(ip >> 24) & 0xFF,
		(ip >> 16) & 0xFF,
		(ip >> 8) & 0xFF,
		(ip) & 0xFF)
}

void* user_handle(user *u)
{
    int newClientIp = ntohl(u->addr.ipv4Addr.sin_addr.s_addr);

	SRV_LOG("Nouveau client ! Addresse: %d.%d.%d.%d",
		(newClientIp >> 24) & 0xFF,
		(newClientIp >> 16) & 0xFF,
		(newClientIp >> 8) & 0xFF,
		(newClientIp) & 0xFF)
	
	user_send_welcome_msg(u);

	if (user_handle_login(u))
	{
		user_handle_msg(u);
	}

	user_handle_logoff(u);
	return NULL;
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