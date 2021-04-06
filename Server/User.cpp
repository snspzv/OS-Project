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
	_connected_uid = p;
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
	std::string packet_text;

	//0 bytes in buffer - client has disconnected
	if(bytes_available == 1 && _state != START)
	{
		_state = DISCONNECTED;
		socket_connected = false;
		printf("In if!!!!\n");
	}

	else
	{
		recv(_uid, messageIn, bytes_available, MSG_DONTWAIT);
		printf("\t\t%s: %s\n", _name, messageIn);
	}

	switch (_state)
	{
		case START:
		{
			send_packet(UNAME_REQUEST, packet_text, TO_SELF);
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
				send_packet(UNAME_VALID | GET_PARTNER, packet_text, TO_SELF);
				memset(_name, 0, sizeof _name);
				strcpy(_name, poten_name);
				_state = ASKING_FOR_CONVO_PARTNER;
			}

			else
			{
				send_packet(UNAME_TAKEN | UNAME_REQUEST, packet_text, TO_SELF);
			}

			break;
		}
			
		
		case ASKING_FOR_CONVO_PARTNER:
		{
			char requested_name[bytes_available] = {};
			strncpy(requested_name, messageIn, strlen(messageIn));
			bool partner_found = false;

			for (auto& user : users)
			{
				if ((strcmp(user.second.get_name(), requested_name) == 0) && (user.first != _uid))
				{
					partner_found = true;
					if (user.second.get_state() != ASKING_FOR_CONVO_PARTNER)
					{
						send_packet(NOT_AVAIL | GET_PARTNER, packet_text, TO_SELF);
					}

					else
					{
						printf("Partner: %s\nSelf: %s\n", std::string(user.second.get_name()).c_str(), std::string(get_name()).c_str());
						send_packet(PARTNER_FOUND, std::string(user.second.get_name()) , TO_SELF);
						_connected_uid = user.second.get_socket();
						send_packet(PARTNER_HAS_REQUESTED, std::string(get_name()), TO_PARTNER);
						user.second.set_partner_socket(_uid);
						user.second.set_state(PARTNER_REQUESTING);
						_state = PARTNER_REQUESTED;

					}
					break;
				}
			}

			if (!partner_found)
			{
				send_packet(PARTNER_DNE | GET_PARTNER, packet_text, TO_SELF);
			}
			break;
		}
			
		
		case PARTNER_REQUESTING:
		{
			if ((messageIn[0] == 'y' || messageIn[0] == 'Y') && (strlen(messageIn) == 1))
			{
				send_packet(CONN_MADE, packet_text, TO_SELF);
				send_packet(CONN_MADE, packet_text, TO_PARTNER);
				users[_connected_uid].set_state(STARTING_P2P);
				_state = STARTING_P2P;
			}

			else if ((messageIn[0] == 'n' || messageIn[0] == 'N') && (strlen(messageIn) == 1))
			{
				send_packet(CONN_DENIED | GET_PARTNER, packet_text, TO_PARTNER);
				users[_connected_uid].set_state(ASKING_FOR_CONVO_PARTNER);
				send_packet(GET_PARTNER, packet_text, TO_SELF);
				_state = ASKING_FOR_CONVO_PARTNER;
			}

			else
			{
				send_packet(BAD_RESPONSE, packet_text, TO_SELF);
			}
			break;
		}
			
		case STARTING_P2P:
		{
			printf("Sending user message\n");
			send_packet(USER_MESSAGE, std::string(messageIn), TO_PARTNER);
			break;
		}
			
		case DISCONNECTED:
		{
			//Is currently in conversation
			if (_connected_uid != 0)
			{
				printf("%s disconnecting with %s!\n", _name, users[_connected_uid].get_name());
				users[_connected_uid].set_state(ASKING_FOR_CONVO_PARTNER);
				send_packet(PARTNER_DISCONNECT | GET_PARTNER, packet_text, TO_PARTNER);
			}

			
		}
			
	}

	return socket_connected;
}

void User::send_packet(uint16_t code_mask, std::string additional_info, bool to_self)
{
	std::string to_send;
	int fd = to_self ?  _uid : _connected_uid;

	to_send.push_back(uint8_t(code_mask >> 8));
	to_send.push_back(uint8_t(code_mask));
	to_send.append(additional_info);
	printf("Packet Length: %ld\n", to_send.length());
	send(fd, to_send.c_str(), to_send.length(), 0);
}