#ifndef USER_H
#define USER_H
#include "server.h"

#include <sys/socket.h>
#include <netinet/in.h>

typedef struct user_t
{
	srv_handle serverHandle;
	int socketHandle;
	socklen_t addrLength;

	/* Pourquoi alloué dynamiquement ? C'est inutile. */
	/* J'ai une bien meilleure idée à la place, utiliser une union ! */
	union
	{
		struct sockaddr_in ipv4Addr;
		struct sockaddr_in6 ipv6Addr;
	} addr;
} user;

user *user_accept(srv_handle s);
void user_handle(user *u);
void user_close(user *u);
void user_free(user*);

#endif /* ifndef USER_H */
