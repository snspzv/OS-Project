#include "User.h"
#include <cstring>
#include <string>
#include <iostream>
#include "Constants.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <vector>
#include <sys/select.h>
#include <mutex>
#include "wait.h"
#include "clientStates.h"
#include <map>

extern std::map<int, User> users;

User::User(int socket_fd)
{
	_uid = socket_fd;
	_connected_uid = 0;
	memset(_name, 0, sizeof _name);
	_state = START;
	_request_uid = 0;
}

User::User()
{
	_uid = 0;
	_connected_uid = 0;
}

char* User::get_name()
{
	return _name;
}

int User::get_socket()
{
	return _uid;
}

int User::get_partner_socket()
{
	return _connected_uid;
}

void User::set_partner_socket(int p)
{
	_request_uid = p;
}

void User::set_state(int state)
{
	_state = state;
}

int User::get_state()
{
	return _state;
}

void User::handleIncoming()
{
	char messageOut[BUFFER_SIZE] = {};
	char messageIn[BUFFER_SIZE] = {};
	recv(_uid, messageIn, BUFFER_SIZE, MSG_PEEK | MSG_DONTWAIT);
	printf("Message Size: %ld\n", strlen(messageIn));
	if ((strlen(messageIn) == 0) && (_state != START))
	{
		_state = DISCONNECTED;
	}
	//printf("Beginning State: %d\n", _state);
	switch (_state)
	{
		case START:
		{
			//printf("Start %d\n", _uid);
			messageOut[0] = UNAME_REQUEST;
			send(_uid, messageOut, strlen(messageOut), 0);
			_state = ASKING_FOR_USERNAME;
			break;
		}

		
		case ASKING_FOR_USERNAME:
		{
			read(_uid, messageIn, BUFFER_SIZE);
			char poten_name[BUFFER_SIZE];
			memset(poten_name, 0, BUFFER_SIZE);
			strncpy(poten_name, messageIn, strlen(messageIn) -1);
			bool unique_name = true;
			for (auto& user : users)
			{
				if (strcmp(poten_name, user.second.get_name()) == 0)
				{
					unique_name = false;
					break;
				}
			}

			if (unique_name)
			{
				memset(_name, 0, sizeof _name);
				strcpy(_name, poten_name);
				messageOut[0] = UNAME_VALID;
				send(_uid, messageOut, strlen(messageOut), 0);
				messageOut[0] = GET_PARTNER;
				sleep(1);
				send(_uid, messageOut, strlen(messageOut), 0);
				_state = ASKING_FOR_CONVO_PARTNER;
			}

			else
			{
				messageOut[0] = UNAME_TAKEN;
				send(_uid, messageOut, strlen(messageOut), 0);
				messageOut[0] = UNAME_REQUEST;
				sleep(1);
				send(_uid, messageOut, strlen(messageOut), 0);
			}

			break;
		}
			
		
		case ASKING_FOR_CONVO_PARTNER:
		{
			printf("\t\t\tAsking for convo partner %d\n", _uid);
			read(_uid, messageIn, BUFFER_SIZE);
			
			char requested_name[BUFFER_SIZE];
			memset(requested_name, 0, sizeof requested_name);
			strncpy(requested_name, messageIn, strlen(messageIn) - 1);
			printf("\t\t\tAfter reading\n");
			bool partner_found = false;

			for (auto& user : users)
			{
				if (strcmp(user.second.get_name(), requested_name) == 0 && user.first != _uid)
				{
					partner_found = true;
					if (user.second.get_state() != ASKING_FOR_CONVO_PARTNER)
					{
						messageOut[0] = NOT_AVAIL;
						send(_uid, messageOut, strlen(messageOut), 0);
						sleep(1);
						messageOut[0] = GET_PARTNER;
						send(_uid, messageOut, strlen(messageOut), 0);
					}

					else
					{
						messageOut[0] = PARTNER_FOUND;
						strcat(messageOut, user.second.get_name());
						send(_uid, messageOut, strlen(messageOut), 0);
						memset(messageOut, 0, sizeof messageIn);
						messageOut[0] = PARTNER_HAS_REQUESTED;
						strcat(messageOut, get_name());
						send(user.second._uid, messageOut, strlen(messageOut), 0);
						user.second.set_partner_socket(_uid);
						user.second.set_state(PARTNER_REQUESTING);
						_state = PARTNER_REQUESTED;
						_connected_uid = user.second.get_socket();
						printf("_uid: %d\n_connected_uid: %d\n", _uid, _connected_uid);
					}
					break;
				}
			}

			if (!partner_found)
			{
				messageOut[0] = PARTNER_DNE;
				send(_uid, messageOut, strlen(messageOut), 0);
				sleep(1);
				messageOut[0] = GET_PARTNER;
				send(_uid, messageOut, strlen(messageOut), 0);
			}
			break;
		}
			
		
		case PARTNER_REQUESTING:
		{
			read(_uid, messageIn, BUFFER_SIZE);
			if (messageIn[0] == 'y' || messageIn[0] == 'Y')
			{
				messageOut[0] = CONN_MADE;
				send(_uid, messageOut, strlen(messageOut), 0);
				_connected_uid = _request_uid;
				send(_connected_uid, messageOut, strlen(messageOut), 0);
				users[_connected_uid].set_state(STARTING_P2P);
				_state = STARTING_P2P;
			}

			else if (messageIn[0] == 'n' || messageIn[0] == 'N')
			{
				messageOut[0] = CONN_DENIED;
				send(_connected_uid, messageOut, strlen(messageOut), 0);
				sleep(1);
				messageOut[0] = GET_PARTNER;
				send(_connected_uid, messageOut, strlen(messageOut), 0);
				users[_connected_uid].set_state(ASKING_FOR_CONVO_PARTNER);
				send(_uid, messageOut, strlen(messageOut), 0);
				_state = ASKING_FOR_CONVO_PARTNER;
			}
			break;
		}
			
		case STARTING_P2P:
		{
			//write user's message into message buffer
			read(_uid, messageIn, BUFFER_SIZE);

			//Send contents of message buffer to partner
			send(_connected_uid, messageIn, strlen(messageIn), 0);
			break;
		}
			
		//case DISCONNECTED:
		//	//Is currently in conversation
		//	if (_connected_uid != 0)
		//	{
		//		mem
		//		send(_connected_uid, )
		//	}
				
	}
	//printf("Ending State: %d\n", _state);
}
