#include "Output.h"
#include "Constants.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iomanip>
extern std::string log_file;

void write_to_log(std::string message, int message_fd, std::string name, bool from_self)
{
	//char message[smessage.length()] = smessage.c_str();
	std::string tabs = "\t\t\t\t\t\t";
	int output_i(0), name_i(0), message_i(0);
	int chars = (message.length() + name.length() + 2);
	int lines = chars / CHAR_LIMIT;
	std::fstream fs;
	fs.open(log_file, std::fstream::app | std::fstream::out);
	//char output[50];
	//Message contents, name, space and colon, newlines
	//fs.flags(std::ios::left);

	message.reserve(message.length() + name.length() + lines + 2);
	message.insert(0, name + ": ");

	if(from_self)
	{
		message.reserve(message.length() + (tabs.length() * lines));
		while(lines > 0)
		{
			message.insert(CHAR_LIMIT * lines - 1,  "\n" + tabs);
			lines--;
		}
		message.insert(0, tabs);
	}

	else
	{
		while(lines > 0)
		{
			message.insert(CHAR_LIMIT * lines, 1, '\n');
			lines--;
		}
		
		//fs.flags(std::ios::left);
	}

	fs << std::left << message << std::endl;


	// fs << name + ": " << message << std::endl;

	fs.close();
	// while (lines >= 0)
	// {
	// 	//clear output buffer
	// 	// memset(output, 0, sizeof output);

	// 	//Tab 7 times if own message
	// 	if (from_self)
	// 	{
	// 		output.append(tabs); 
	// 		// write(message_fd, tabs, strlen(tabs));
	// 	}

	// 	//copy name to output buffer
	// 	while (name_i < (name.length() + 2))
	// 	{
	// 		//insert newline if output buffer is on last character and start loop over
	// 		if (output_i == 49)
	// 		{
	// 			output[output_i] = '\n';
	// 			output_i = 0;
	// 			break;
	// 		}

	// 		else if (output_i == name.length())
	// 		{
	// 			output[output_i] = ':';
	// 		}

	// 		else if (output_i == (name.length() + 1))
	// 		{
	// 			output[output_i] = ' ';
	// 		}

	// 		//normal case - copy char from name buffer to output buffer 
	// 		else
	// 		{
	// 			output[output_i] = name[name_i];
	// 		}
			
	// 		output_i++;
	// 		name_i++;
	// 	}

	// 	//copy message to output buffer
	// 	while ((message_i < message.length()) && (name_i == name.length() + 2))
	// 	{
	// 		//insert newline if output buffer is on last character and start loop over
	// 		if (output_i == 49)
	// 		{
	// 			output[output_i] = '\n';
	// 			output_i = 0;
	// 			// write(message_fd, output, strlen(output));
	// 			// fdatasync(message_fd);
	// 			// memset(output, 0, sizeof message);
	// 			break;
	// 		}

	// 		//normal case - copy char from message buffer to output buffer and increment both
	// 		output[output_i] = message[message_i];
	// 		output_i++;
	// 		message_i++;
	// 	}

	// 	lines--;
		

	// }
		
	
	//output not written on last line because less than 49 characters
	// if (output_i != 0)
	// {
	// 	output[output_i] = '\u000A';
	// 	write(message_fd, output, strlen(output));
	// 	fdatasync(message_fd);
	// 	memset(output, 0, sizeof message);
	// }
	// message.append("\n");
	// write(message_fd, message.c_str(), message.length());
	// fdatasync(message_fd);
}

