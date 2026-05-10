#pragma once

/* Ryane Menaï 12409318
 * Je déclare qu'il s'agit de mon propre travail.
 * Ce travail a été réalisé intégralement par un être humain. */

#include <stdbool.h>
#include <inttypes.h>

#define CLT_NULL_HANDLE NULL

typedef void* clt_handle;

/** Alloue une instance d'un client sur le tas.
 * Retourne un pointer vers une structure opaque permettant d'interferer avec
 * la structure sous-jacente. */
clt_handle clt_alloc();

/** Essaie de convertir stringAddr en entier et place le resultat dans outAddr.
 * retourne false en cas d'erreur, true sinon. */
bool clt_try_parse_ipv4_address(const char *stringAddr, unsigned int *outAddr);

/** Retourne true si line est vide, false sinon. */
bool clt_check_empty_line(const char *line);

/** Permet de sauter un caractère null dans le buffer de la socket s'il y en a un. */
void clt_try_skip_null_char(clt_handle handle);

/** Lit au plus n octets depuis le buffer de la socket
 * et place le resultat dans data. Retourne false en cas d'erreur,
 * true sinon. */
bool clt_read_socket(clt_handle handle, char *data, int n);

/** Lit au plus n octets depuis le buffer de l'entrée
 * et place le resultat dans data. Retourne false en cas d'erreur,
 * true sinon. */
bool clt_read_input(clt_handle handle, char *data, int n);

/** Initialise le client et le connecte au serveur.
 * Retourne false en cas d'erreur, true sinon. */
bool clt_init(clt_handle handle);

/** Execute la procédure pour recevoir le message de bienvenue.
 * Retourne false en cas d'erreur, true sinon. */
bool clt_receive_welcome_msg(clt_handle handle);

/** Execute la procédure pour obtenir un nom d'utilisateur.
 * Retourne false en cas d'erreur, true sinon. */
bool clt_login(clt_handle handle);

/** Lit la socket pour recevoir le prochain message et l'affiche à l'écran.
 * Retourne false en cas d'erreur, true sinon. */
bool clt_handle_server_msg(clt_handle handle);

/** Lit l'entrée et l'envoie au serveur.
 * Retourne false en cas d'erreur, true sinon. */
bool clt_handle_input_msg(clt_handle handle);

/** Procédure de mise à jour du client. Essaie de lire l'entrée et la socket.
 * Retourne true s'il faut continuer l'execution, false sinon. */
bool clt_update(clt_handle handle);

/** Procédure d'execution du client, gere l'entireté du cycle de vie
 * d'un client après clt_init et avant clt_close. */
void clt_run(clt_handle handle);

/** Ferme la connexion entre le client et le serveur et libère les ressources. */
void clt_close(clt_handle handle);

/** Se connecter au serveur TCP d'adresse donnée en argument sous forme de
 * chaîne de caractère et au port donné en argument
 * retourne le descripteur de fichier de la socket obtenue ou -1 en cas
 * d'erreur. */
int clt_connect_to_server(char *adresse, uint16_t port);
