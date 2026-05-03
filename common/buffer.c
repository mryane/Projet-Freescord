#include "buffer.h"

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct buffer {
	int fd;
	/* autres champs, à vous */
};

buffer *buff_create(int fd, size_t buffsz)
{
	/* pour éviter les warnings de variable non utilisée */
	buffsz = fd = 0;
	return NULL;
}

int buff_getc(buffer *b)
{
	/* pour éviter les warnings de variable non utilisée */
	b = NULL;
	return 0;
}

int buff_ungetc(buffer *b, int c)
{
	/* pour éviter les warnings de variable non utilisée */
	b = NULL;
	return c;
}

void buff_free(buffer *b)
{
	/* pour éviter les warnings de variable non utilisée */
	b = NULL;
}

int buff_eof(const buffer *buff)
{
	/* pour éviter les warnings de variable non utilisée */
	return buff == NULL;
}

int buff_ready(const buffer *buff)
{
	/* pour éviter les warnings de variable non utilisée */
	return buff == NULL;
}

char *buff_fgets(buffer *b, char *dest, size_t size)
{
	/* pour éviter les warnings de variable non utilisée */
	b = NULL; size = 0;
	return dest;
}

char *buff_fgets_crlf(buffer *b, char *dest, size_t size)
{
	/* pour éviter les warnings de variable non utilisée */
	b = NULL; size = 0;
	return dest;
}
