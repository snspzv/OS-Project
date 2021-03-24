#include "tx.h"
#include "Constants.h"
#include "Output.h"
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string>
#include <iostream>
extern int log_fd;

int outgoing(int sock_fd, bool tx_user_message, char *name, bool & entering_name, char * partner_name)
{
	//char message[1024];
	//memset(message, 0, 1024);
	int ret;
	std::string message;
	std::getline(std::cin, message);
	//Read message from STDIN and write to message buffer
	//read(STDIN_FILENO, message, sizeof message);
	
	//Checking if name has not been sent, means this time name has been sent
	if (name[0] == 0)
	{
		strncpy(name, message.c_str(), message.length());
		printf("My name: %s with length of: %ld\n", name, strlen(name));
		ret =  RX_NEXT;
	}

	else if (entering_name)
	{
		memset(partner_name, 0, sizeof partner_name);
		strncpy(partner_name, message.c_str(), message.length());
		entering_name = false;
		ret =  RX_NEXT;
	}

	else if(tx_user_message)
	{
		//Write contents of message buffer to log
		write_to_log(message.c_str(), log_fd, name, false);
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
