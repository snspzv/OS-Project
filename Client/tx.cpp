#include "tx.h"
#include "Constants.h"
#include "Output.h"
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <fstream>
extern int log_fd;
extern std::string name;
extern std::string partner_name;
extern std::string log_file;
extern std::string top;
extern std::string bottom;
int outgoing(int sock_fd, bool tx_user_message, bool & entering_name)
{
	//char message[1024];
	//memset(message, 0, 1024);
	int ret;
	std::string message;
	std::getline(std::cin, message);
	//Read message from STDIN and write to message buffer
	//read(STDIN_FILENO, message, sizeof message);
	
	//Checking if name has not been sent, means this time name has been sent
	if (name.empty())
	{
		//strncpy(name, message.c_str(), message.length());
		name = message;
		ret =  RX_NEXT;
	}

	else if (entering_name)
	{
		// memset(partner_name, 0, sizeof partner_name);
		// strncpy(partner_name, message.c_str(), message.length());
		partner_name = message;
		entering_name = false;
		ret =  RX_NEXT;
	}

	else if(tx_user_message)
	{
		write_to_log(message, log_fd, name, true);
		// printf("\t\t\t\tTo %s\n-------------------------------------------------------------------------------------------\n", partner_name.c_str());
		// system((std::string("tail -n 20 ") + log_file).c_str());
		// printf("\n-------------------------------------------------------------------------------------------\n-> ");
		// fflush(stdout);
		std::fstream fs;
		fs.open(log_file, std::fstream::in);
		std::cout << top << fs.rdbuf() << bottom << std::flush;
		fs.close();
		ret = EITHER_NEXT;
	}

	//Answering if they want to connect with user or not
	else
	{
		ret = RX_NEXT;
	}

	//Send contents of message buffer to server
	send(sock_fd, message.c_str(), message.length(), 0);

	return ret;
}
