/* Ryane Menaï 12409318
 * Je déclare qu'il s'agit de mon propre travail.
 * Ce travail a été réalisé intégralement par un être humain. */

#include "common/utils.h"

char *crlf_remove(char *line_with_crlf)
{
	char *ptr = line_with_crlf;

	while (*ptr != '\r' && *ptr != '\0')
	{
		if (*(ptr + 1) == '\n')
		{
			break;
		}

		ptr++;
	}

	if (ptr[0] != '\r' || ptr[1] != '\n')
	{
		return 0;
	}

	ptr[0] = '\0';
	ptr[1] = '\0';

	return line_with_crlf;
}

char *crlf_to_lf(char *line_with_crlf)
{
	char *ptr = line_with_crlf;

	while (*ptr != '\r' && *ptr != '\0')
	{
		if (*(ptr + 1) == '\n')
		{
			break;
		}

		ptr++;
	}

	if (ptr[0] != '\r' || ptr[1] != '\n')
	{
		return 0;
	}

	ptr[0] = '\n';
	ptr[1] = '\0';

	return line_with_crlf;
}

char *lf_to_crlf(char *line_with_lf)
{
	char *ptr = line_with_lf;

	while (*ptr != '\n' && *ptr != '\0')
	{
		ptr++;
	}

	if (*ptr != '\n')
	{
		return 0;
	}

	ptr[0] = '\r';
	ptr[1] = '\n';
	ptr[2] = '\0';

	return line_with_lf;
}
