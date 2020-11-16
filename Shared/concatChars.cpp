#include "concatChars.h"
#include <cstring>

void concatChars(char* dest, char* first, char* second)
{
	int index = 0;

	for (int i = 0; i < strlen(first); i++, index++)
	{
		dest[i] = first[i];
	}

	for (int i = 0; i < strlen(second); i++, index++)
	{
		dest[i + strlen(first)] = second[i];
	}

	dest[index] = '\0';
}
