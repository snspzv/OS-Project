#include "Output.h"
#include "Constants.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

void write_to_log(char* message, int message_fd, char* name, bool from_self)
{
	char tabs[7];
	int output_i(0), name_i(0), message_i(0);
	int chars = (strlen(message) + strlen(name) + 2);
	int lines = chars / CHAR_LIMIT;
	char output[50];

	memset(tabs, '\t', sizeof tabs);
	printf("%s\n", message);
	while (lines >= 0)
	{
		//clear output buffer
		memset(output, 0, sizeof output);

		//Tab 7 times if own message
		if (from_self)
		{
			write(message_fd, tabs, strlen(tabs));
		}

		//copy name to output buffer
		while (name_i < (strlen(name) + 2))
		{
			//insert newline if output buffer is on last character and start loop over
			if (output_i == 49)
			{
				output[output_i] = '\n';
				output_i = 0;
				//lseek(message_fd, -4, SEEK_CUR);
				break;
			}

			else if (output_i == strlen(name))
			{
				output[output_i] = ':';
			}

			else if (output_i == (strlen(name) + 1))
			{
				output[output_i] = ' ';
			}

			//normal case - copy char from name buffer to output buffer 
			else
			{
				output[output_i] = name[name_i];
			}
			
			output_i++;
			name_i++;
		}

		//copy message to output buffer
		while ((message_i < strlen(message)) && (name_i == strlen(name) + 2))
		{
			//insert newline if output buffer is on last character and start loop over
			if (output_i == 49)
			{
				output[output_i] = '\u000A';
				output_i = 0;
				write(message_fd, output, strlen(output));
				fdatasync(message_fd);
				memset(output, 0, sizeof message);
				lseek(message_fd, -4, SEEK_CUR);
				break;
			}

			//normal case - copy char from message buffer to output buffer and increment both
			output[output_i] = message[message_i];
			output_i++;
			message_i++;
		}

		lines--;
		

	}
		
	
	//output not written on last line because less than 49 characters
	if (output_i != 0)
	{
		output[output_i] = '\u000A';
		write(message_fd, output, strlen(output));
		fdatasync(message_fd);
		memset(output, 0, sizeof message);
		//lseek(message_fd, -4, SEEK_CUR);
	}

}
