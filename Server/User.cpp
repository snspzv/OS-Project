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
#include "clientStates.h"
#include <map>
#include <sys/ioctl.h>

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

bool User::handleIncoming()
{
	char messageOut[BUFFER_SIZE] = {};
	bool socket_connected = true;
	int bytes_available = 0;
	int bytes = ioctl(_uid, FIONREAD, &bytes_available);
	bytes_available++;
	char messageIn[bytes_available] = {};
	printf("Bytes: %d\tBytes available: %d\n", bytes, bytes_available);
	if(bytes_available == 1 && _state != START)
	{
		_state = DISCONNECTED;
		socket_connected = false;
	}

	else
	{
		recv(_uid, messageIn, bytes_available, MSG_DONTWAIT);
	}

	switch (_state)
	{
		case START:
		{
			messageOut[0] = UNAME_REQUEST;
			send(_uid, messageOut, strlen(messageOut), 0);
			_state = ASKING_FOR_USERNAME;
			break;
		}

		
		case ASKING_FOR_USERNAME:
		{
			char poten_name[bytes_available] = {};
			strncpy(poten_name, messageIn, strlen(messageIn));
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
			char requested_name[bytes_available];
			memset(requested_name, 0, sizeof requested_name);
			strncpy(requested_name, messageIn, strlen(messageIn));
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
			//read(_uid, messageIn, BUFFER_SIZE);
			if ((messageIn[0] == 'y' || messageIn[0] == 'Y') && (strlen(messageIn) == 1))
			{
				messageOut[0] = CONN_MADE;
				send(_uid, messageOut, strlen(messageOut), 0);
				_connected_uid = _request_uid;
				send(_connected_uid, messageOut, strlen(messageOut), 0);
				users[_connected_uid].set_state(STARTING_P2P);
				_state = STARTING_P2P;
			}

			else if ((messageIn[0] == 'n' || messageIn[0] == 'N') && (strlen(messageIn) == 1))
			{
				messageOut[0] = CONN_DENIED;
				send(_request_uid, messageOut, strlen(messageOut), 0);
				sleep(1);
				messageOut[0] = GET_PARTNER;
				send(_request_uid, messageOut, strlen(messageOut), 0);
				users[_request_uid].set_state(ASKING_FOR_CONVO_PARTNER);
				send(_uid, messageOut, strlen(messageOut), 0);
				_state = ASKING_FOR_CONVO_PARTNER;
			}

			else
			{
				messageOut[0] = BAD_RESPONSE;
				send(_uid, messageOut, strlen(messageOut), 0);
			}
			break;
		}
			
		case STARTING_P2P:
		{
			//write user's message into message buffer
			//read(_uid, messageIn, BUFFER_SIZE);

			//Send contents of message buffer to partner
			send(_connected_uid, messageIn, strlen(messageIn), 0);
			break;
		}
			
		case DISCONNECTED:
			
			//Is currently in conversation
			if (_connected_uid != 0)
			{
				messageOut[0] = PARTNER_DISCONNECT;
				users[_connected_uid].set_state(ASKING_FOR_CONVO_PARTNER);
				send(_connected_uid, messageOut, strlen(messageOut), 0);
				messageOut[0] = GET_PARTNER;
				sleep(1);
				send(_connected_uid, messageOut, strlen(messageOut), 0);
			}

			//Just been requested for conversation
			else if(_request_uid != 0)
			{
				messageOut[0] = PARTNER_DISCONNECT;
				users[_request_uid].set_state(ASKING_FOR_CONVO_PARTNER);
				send(_request_uid, messageOut, strlen(messageOut), 0);
				messageOut[0] = GET_PARTNER;
				sleep(1);
				send(_request_uid, messageOut, strlen(messageOut), 0);
			}

			_request_uid = 0;
			_connected_uid = 0;
	}

	return socket_connected;
}
