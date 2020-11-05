#include "User.h"

User::User(int socket_fd)
{
	_uid = socket_fd; //each new socket will have unique file descriptor
}

//constructor for when array is first declared
User::User()
{
	_uid = 0;
}