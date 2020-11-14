#include "User.h"
#include <cstring>
#include <iostream>
#include "Constants.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <vector>
#include <sys/select.h>

User::User(int socket_fd)
{
	_uid = socket_fd; //each new socket will have unique file descriptor
	_connected_uid = 0;
	//strncpy(_name, name, strlen(name));
	_connection_requested = false;
}

//constructor for when array is first declared
User::User()
{
	_uid = 0;
	_connected_uid = 0;
}

bool User::select_user(int new_socket, std::vector<User> p_vector, fd_set & fds, timespec & tv)
{
	char from_server[BUFFER_SIZE] = "Wait for a connection or enter the name of the user you'd like to message: ";
	char error_mess[BUFFER_SIZE] = "Invalid name. Try again\n";
	char successful_con[BUFFER_SIZE] = "Connection Successful.";
	char name[BUFFER_SIZE];
	int status;

	send(new_socket, from_server, strlen(from_server), 0);

	//Continue looping until client sends name of user they want to chat with OR connection requested is true
	do
	{
		status = pselect(new_socket + 1, &fds, NULL, NULL, &tv, NULL);
		printf("%d\n", status);
	} while ((status == 0) && (_connection_requested == false));

	
	//another user has asked to message with current user
	if (_connection_requested)
	{
		status = 1;
		//Ask user if they want to message requesting user
		//if not exit with false
	}

	//pselect returns error
	else if (status == -1)
	{
		perror("select");
		return false;
	}

	//pselect has returned number of file descriptors in fds that are ready to be read
	//only fd in fds is new_socket so no reason to check number for others
	//Indicates user has sent the name of a user they want to message
	else
	{
		read(new_socket, name, BUFFER_SIZE);

		for (auto& pot_user : p_vector)
		{
			if ((strcmp(pot_user._name, name) == 0) && (pot_user._uid != _uid))
			{
				_connected_uid = pot_user._uid;
				pot_user._connected_uid = _uid;
				send(new_socket, successful_con, strlen(successful_con), 0);
				return true;
			}
		}
		send(new_socket, error_mess, strlen(error_mess), 0);
	}
	
	return false;
}

void User::enter_name(int new_socket, std::vector<User> p_vector)
{
	char enter_name[BUFFER_SIZE] = "Enter your username: ";
	char not_unique[BUFFER_SIZE] = "The username you entered is already taken. Try again.";
	char unique[BUFFER_SIZE] = "Valid username!";
	char name[BUFFER_SIZE];
	bool valid_name = true;

	do
	{
		valid_name = true;
		send(new_socket, enter_name, strlen(enter_name), 0);
		read(new_socket, name, BUFFER_SIZE);
		
		for(auto& pot_user : p_vector)
		{
			if((strcmp(pot_user._name, name) == 0))
			{
				
				valid_name = false;
				send(new_socket, not_unique, strlen(not_unique), 0);
				break;
			}
		}

		if (valid_name)
		{
			send(new_socket, unique, strlen(unique), 0);
		}
	} while(!valid_name);


	strncpy(_name, name, strlen(name));

	return;
}

char* User::get_name()
{
	return _name;
}
