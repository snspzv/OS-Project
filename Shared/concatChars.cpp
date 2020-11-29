#include "concatChars.h"
#include "Constants.h"
#include <cstring>

//general version, not for writing to message file
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


/*void concatChars(char* dest, char* name, char* message, bool partner_message)
{
	int index = 0;
	int nl_count = 0;
	char tabs[7] = "\t\t\t\t\t\t\t";

	if (partner_message)
	{
		for
	}
}*/