/* Fichier de test pour common/buffer.c */
#include "common/buffer.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int numArgs, const char **args)
{
    buffer *myBuffer = buff_create(STDIN_FILENO, 4096);

    char dummy[8192];

    buff_fgets_crlf(myBuffer, dummy, 8192);
    printf("Dummy -> %s\n", dummy);

    return EXIT_SUCCESS;
}