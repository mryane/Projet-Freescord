/* Ryane Menaï 12409318
 * Je déclare qu'il s'agit de mon propre travail.
 * Ce travail a été réalisé intégralement par un être humain. */

#include "common/buffer.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

struct buffer {
	int fd;
	int isEof;
	int isError;
	size_t bufferSize;
	size_t bufferOffset;
	size_t lastReadSize;
	char* bufferData;
};

buffer *buff_create(int fd, size_t buffsz)
{
	buffer *newBuffer = (buffer*) malloc(sizeof(buffer));

	newBuffer->fd = fd;
	newBuffer->isEof = 0;
	newBuffer->isError = 0;
	newBuffer->bufferSize = buffsz;
	newBuffer->bufferOffset = buffsz;
	newBuffer->lastReadSize = 0;
	newBuffer->bufferData = (char*) malloc(buffsz);

	return newBuffer;
}

void buff_free(buffer *b)
{
	free(b->bufferData);
	free(b);
}

void buff_fill(buffer *b)
{
	if (b->isError)
	{
		return;
	}

	if (b->bufferOffset >= b->lastReadSize)
	{
		b->lastReadSize = read(b->fd, b->bufferData, b->bufferSize);

		if (b->lastReadSize == 0)
		{
			b->isEof = 1;
		}
		else if (b->lastReadSize == -1)
		{
			perror("[BUFFER] Erreur lors de la lecture");
			b->isError = 1;
		}
		else
		{
			b->bufferOffset = 0;
		}
	}
}

int buff_getc(buffer *b)
{
	buff_fill(b);

	if (buff_eof(b))
	{
		return EOF;
	}

	return b->bufferData[b->bufferOffset++];
}

int buff_ungetc(buffer *b, int c)
{
	if (b->bufferOffset < 1 || b->bufferData[b->bufferOffset - 1] != c)
	{
		printf("[BUFFER] Vous avez tentez de unget le mauvais caractère, "
			"ou vous n'avez pas encore lu dans le buffer.");
		return 0;
	}

	b->bufferOffset--;
	return c;
}

int buff_eof(const buffer *b)
{
	return b->isEof && (b->bufferOffset == b->bufferSize);
}

int buff_error(const buffer *b)
{
	return b->isError;
}

int buff_ready(const buffer *b)
{
	if (buff_eof(b))
	{
		return 0;
	}

	return b->bufferOffset < b->lastReadSize;
}

char *buff_fgets(buffer *b, char *dest, size_t size)
{
	if (buff_eof(b))
	{
		return NULL;
	}

	size_t i = 0;
	char c;

	while (i < size - 1)
	{
		c = buff_getc(b);

		if (buff_error(b))
		{
			return NULL;
		}

		if (c == EOF)
		{
			break;
		}

		dest[i++] = c;

		if (c == '\n')
		{
			break;
		}
	}

	dest[i] = '\0';
	return dest;
}

char *buff_fgets_crlf(buffer *b, char *dest, size_t size)
{
	if (buff_eof(b))
	{
		return NULL;
	}

	size_t i = 0;
	char c;
	int isLastCharCr = 0;

	while (i < size - 1)
	{
		c = buff_getc(b);

		if (buff_error(b))
		{
			return NULL;
		}

		if (c == EOF)
		{
			break;
		}

		dest[i++] = c;

		if (c == '\n' && isLastCharCr)
		{
			break;
		}

		isLastCharCr = c == '\r';
	}

	dest[i] = '\0';
	return dest;
}
