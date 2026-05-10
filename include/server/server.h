#pragma once
#include <stdbool.h>
#include <inttypes.h>

struct user_t;
typedef void* srv_handle;

#define SRV_NULL_HANDLE NULL

/** Alloue une instance d'un serveur sur le tas.
 * Retourne un pointer vers une structure opaque permettant d'interferer avec
 * la structure sous-jacente. */
srv_handle srv_alloc();

/** Initialise le serveur, créer entre autre la socket d'écoute
 * ainsi que la liste des utilisateurs. Retourne false en cas d'erreur, true sinon. */
bool srv_init(srv_handle);

/** Retourne le descripteur de fichier de la socket du serveur. */
int srv_get_socket(srv_handle);

/** Retourne un descripteur de fichier qui correspond à l'entrée en écriture d'un tube. */
int srv_get_write_pipe(srv_handle);

/** Retourne la chaine de caractère de bienvenue à envoyer au client après sa connection. */
const char *srv_get_welcome_msg(srv_handle);

/* Retourne true si la chaine de caractères données n'est utilisé que par un seul utilisateur, false sinon */
bool srv_check_unused_nickname(srv_handle, const char *);

/** Retire l'user donné de la liste des utilisateurs du serveur. */
void srv_remove_user(srv_handle, struct user_t*);

/** Procédure d'execution du serveur, tente entre autre d'accepter un nouveau client. */
void srv_tick(srv_handle handle);

/** Procédure du thread de répétition. Permet de broadcast les messages des clients. */
void* srv_repeat(srv_handle handle);

/** Ferme la connexion entre le serveur et tous les clients connectés et libère les ressources. */
void srv_close(srv_handle);

/** Créer et configurer une socket d'écoute sur le port donné en argument
 * retourne le descripteur de cette socket, ou -1 en cas d'erreur */
int srv_create_listening_sock(uint16_t port);
