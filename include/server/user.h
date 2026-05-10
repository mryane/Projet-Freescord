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

	char nickname[24];
} user;

/** Accepte et initialise un nouvel utilisateur.
 * Retourne un pointer vers le nouvel utilisateur, NULL en cas d'erreur. */
user *user_accept(srv_handle s);

/** Procédure permettant d'envoyer le message de bienvenue à l'utilisateur. */
void user_send_welcome_msg(user *u);

/** Renvoie true si loginCmd est correctement formaté, false sinon. */
bool user_check_login_cmd(const char *loginCmd);

/** Renvoie vraie si le nickname de l'utilisateur est valide, false sinon. */
bool user_check_nickname(user *u);

/** Envoie à l'utilisateur une réponse d'inscription correspondant à answerCode. */
void user_send_login_answer(user *u, int answerCode);

/** Permet de gérer l'inscription de l'utilisateur dans le serveur,
 * Renvoie false en cas d'erreur, true sinon. */
bool user_handle_login(user *u);

/** Procédure permettant de gérer les messages reçu de l'utilisateur.
 * Elle se termine lorsqu'une erreur est détectée, ou lorsque la connexion
 * a été fermée. */
void user_handle_msg(user *u);

/** Procédure permetant de gérer la deconnexion de l'utilisateur. */
void user_handle_logoff(user *u);

/** Fonction d'execution de l'utilisateur, gère le cycle de vie de l'utilisateur
 *  après user_accept. */
void* user_handle(user *u);

/** Ferme la socket de l'utilisateur. */
void user_close(user *u);

/** Libère les ressources de l'utilisateur. */
void user_free(user *u);

#endif /* ifndef USER_H */
