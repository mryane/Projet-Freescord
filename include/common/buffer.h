#ifndef _BUFFER_H
#define _BUFFER_H
#include <stdlib.h>
#ifndef EOF
#define EOF -1
#endif

/** Lectures dans un fichier avec buffer (tampon)
 *
 * buffer est un type opaque pour la lecture avec tampon dans un fichier
 *
 * Toutes les fonctions de cette bibliothèque commencent par le préfixe "buff_".
 * À part buff_create, elles prennent toutes un pointeur vers un buffer en
 * premier argument (NOTE : cela diffère en cela des fonctions similaires de
 * stdio.h).
 *
 * On ne peut créer un buffer qu'avec buff_create, il faut ensuite le libérer
 * avec buff_free.
 *
 * Les autres primitives sont :
 * - buff_getc pour la lecture d'un caractère (et si besoin, un appel à read
 *   lorsque tous les caractères du buffer ont été consommés)
 * - buff_ungetc qui permet de remettre un caractère dans le tampon, ce qui
 *   n'est garanti que pour un seul caractère après en avoir consommé un
 * - buff_eof qui permet de demander si la lecture a atteint la fin du fichier
 * - buff_ready qui indique s'il reste des caractères à consommer dans le buffer
 *   sans avoir à lire dans le fichier
 *
 * Par ailleurs, les fonctions suivantes permettent de lire ligne par ligne dans
 * le fichier en utilisant le buffer :
 * - buff_fgets pour une ligne terminée par '\n' (LF, line feed)
 * - buff_fgets_crlf pour une ligne terminée par '\r\n' (CRLF, carriage return
 *   and line feed)
 */

typedef struct buffer buffer;

/** Créer un buffer de taille buffsz pour les lectures
 * associées au fichier fd */
buffer *buff_create(int fd, size_t buffsz);

/** Retourner le prochain caractère à lire ou EOF (-1) si la fin
 * du fichier est atteinte.
 *
 * Cette fonction appellera l'appel système read si tous les octets
 * du buffer ont déjà été consommés */
int buff_getc(buffer *b);

/** Remettre le caractère c dans le buffer.
 * Le résultat n'est garanti que pour un seul caractère.
 * retourne le caractère c. */
int buff_ungetc(buffer *b, int c);

/** Libérer le buffer buff et toute la mémoire associée. */
void buff_free(buffer *buff);

/** Retourner 1 si la lecture a atteint la fin du fichier, 0 sinon */
int buff_eof(const buffer *buff);

/** Retourner 1 si des octets sont disponibles dans le buffer sans lecture du
 * fichier */
int buff_ready(const buffer *buff);

/* Lire au plus size - 1 caractère dans le buffer b et les stocker dans le
 * tableau dest qui devrait pouvoir contenir au moins size octets.
 * La lecture s'arrête après une fin de fichier ou le caractère de fin de ligne
 * '\n'. Si une fin de ligne '\n' est lue, elle est stocké dans dest. Retourne
 * buf en cas de succès, NULL en cas d'erreur, ou si la fin du fichier est
 * atteinte sans que des caractères aient été lus. */
char *buff_fgets(buffer *b, char *dest, size_t size);

/* Lire au plus size - 1 caractère dans le buffer b et les stocker dans le
 * tableau dest qui devrait pouvoir contenir au moins size octets.
 * La lecture s'arrête après une fin de fichier ou les deux caractères '\r'
 * suivi de '\n'. Si une fin de ligne "\r\n" est lue, elle est stocké dans dest.
 * Retourne buf en cas de succès, NULL en cas d'erreur, ou si la fin du fichier
 * est atteinte sans que des caractères aient été lus. */
char *buff_fgets_crlf(buffer *b, char *dest, size_t size);

#endif
