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

User::User(int socket_fd, char * name)
{
	_uid = socket_fd; //each new socket will have unique file descriptor
	_connected_uid = 0;
	strncpy(_name, name, strlen(name));
}

//constructor for when array is first declared
User::User()
{
	_uid = 0;
	_connected_uid = 0;
}

bool User::select_user(int new_socket, std::vector<User> p_vector)
{
	char from_server[BUFFER_SIZE] = "Enter the name of the user you'd like to message: ";
	char error_mess[BUFFER_SIZE] = "Invalid name. Try again\n";
	char name[BUFFER_SIZE];

	send(new_socket, from_server, strlen(from_server), 0);
	read(new_socket, name, BUFFER_SIZE);

	for(auto& pot_user : p_vector)
	{
		if(strcmp(pot_user._name, name) == 0)
		{
			_connected_uid = pot_user._uid;
			pot_user._connected_uid = _uid;
			return true;
		}
	}
	send(new_socket, error_mess, strlen(error_mess), 0);
	return false;
}
