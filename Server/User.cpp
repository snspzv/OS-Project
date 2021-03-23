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
#include "sendRecv.h"
#include "sharedInfo.h"
#include "concatChars.h"
#include "wait.h"
#include "clientStates.h"
#include <map>

extern std::map<int, User> users;

User::User(int socket_fd)
{
	_uid = socket_fd; //each new socket will have unique file descriptor
	_connected_uid = 0;
	memset(_name, 0, sizeof _name);
	_connection_requested = false;
	_request_accepted = 0;
	_state = START;
}

//constructor for when array is first declared
User::User()
{
	_uid = 0;
	_connected_uid = 0;
	_request_accepted = 0;
}

bool User::select_user(fd_set & fds, timespec & tv)
{
	char from_server[BUFFER_SIZE] = "Wait for a connection or enter the name of the user you'd like to message: ";
	char conRequested[BUFFER_SIZE] = " has requested to message with you. Do you want to message them?(y/n) ";
	char bad_name[BUFFER_SIZE] = "Invalid name. Try again";
	char successful_con[BUFFER_SIZE] = "Connection Successful.";
	char deny_con[BUFFER_SIZE] = "Connection Denied.";
	char busy[BUFFER_SIZE] = "This user is currently not available :(";
	char timeout_err[BUFFER_SIZE] = "Your request has timed out";
	char invalid_response[BUFFER_SIZE] = "Invalid response. Please answer 'y' or 'n'";
	char asking_partner[BUFFER_SIZE] = " found, asking them now";
	char name[BUFFER_SIZE];
	char yes_no[BUFFER_SIZE];
	char only_code[1];
	int status;
	bool matched = false;
	bool request_ongoing = true;
	int bad_responses = 0;
	fd_set temp_fds;
	tv.tv_sec = 2;

	memset(only_code, 0, 1);
	send_buffer(_uid, only_code, strlen(only_code), false, GET_PARTNER);

	set_not_busy(_vect_index);
	//Continue looping until client sends name of user they want to chat with OR connection requested is true
	do
	{
		temp_fds = fds;
		status = pselect(_uid + 1, &temp_fds, NULL, NULL, &tv, NULL);
		//printf("%d\n", status);
	} while ((status == 0) && (is_not_busy(_vect_index)));


	//another user has asked to message with current user
	if (!is_not_busy(_vect_index))
	{
		//Ask user if they want to message with potential partner
		int requester_index = get_requester_name(_vect_index, name);
		char request_approval[BUFFER_SIZE];
		concatChars(request_approval, name, conRequested);
		send_buffer(_uid, name, strlen(name), false, PARTNER_HAS_REQUESTED);

		do
		{
			//Wait on user to respond for 60 seconds
			temp_fds = fds;
			tv.tv_sec = 60;
			status = pselect(_uid + 1, &temp_fds, NULL, NULL, &tv, NULL);
			//timeout has occurred OR too many more than two invalid responses
			if ((status == 0) || (bad_responses > 2))
			{
				timeout_request(requester_index);
				send_buffer(_uid, only_code, strlen(only_code), false, TIMEOUT);
				sleep(0.5);
				request_ongoing = false;
			}

			//pselect returns error
			else if (status == -1)
			{
				perror("select");
				return false;
			}

			//pselect has returned number of file descriptors in fds that are ready to be read
			//only fd in fds is _uid so no reason to check number for others
			//Indicates user has rejected or accepted request
			else
			{
				receive(_uid, yes_no, BUFFER_SIZE);
				//User accepts partner
				if ((yes_no[0] == 'Y') || (yes_no[0] == 'y'))
				{
					accept_request(_vect_index, requester_index);
					sleep(2);//Give time for other thread to notice and set connection_index
					send_buffer(_uid, only_code, strlen(only_code), false, CONN_MADE);
					matched = true;
					request_ongoing = false;
				}

				//User denies partner
				else if ((yes_no[0] == 'N') || (yes_no[0] == 'n'))
				{
					deny_request(requester_index);
					send_buffer(_uid, only_code, strlen(only_code), false, CONN_DENIED);
					sleep(0.5);
					request_ongoing = false;
				}

				//Invalid response
				else
				{
					send_buffer(_uid, only_code, strlen(only_code), false, BAD_RESPONSE);
					bad_responses++;
				}
			}
		} while (request_ongoing);
	}

	//pselect returns error
	else if (status == -1)
	{
		perror("select");
		return false;
	}

	//pselect has returned number of file descriptors in fds that are ready to be read
	//only fd in fds is _uid so no reason to check number for others
	//Indicates user has sent the name of a user they want to message
	else
	{
		receive(_uid, name, BUFFER_SIZE);

		int partner_index = name_in_set(name, _uid);
		printf("%d\n", partner_index);
		//Potential partner found
		if (partner_index != -1)
		{
			//Partner is busy and cannot be requested
			if (!request_partner(_vect_index, partner_index))
			{
				send_buffer(_uid, only_code , strlen(only_code), false, NOT_AVAIL);
				sleep(1);
			}

			//Partner available and request flag set
			else
			{
				send_buffer(_uid, only_code, strlen(only_code), false, PARTNER_FOUND);

				//Continue checking if own connection_requested written to by potential partner or timeout
				//It's initialized as its index in infoShare vector
				do
				{
					sleep(1);
					status = check_request_status(_vect_index);
					
				} while (status == _vect_index);
				
				//Potential partner has denied request
				if (status == -1)
				{
					send_buffer(_uid, only_code, strlen(only_code), false, CONN_DENIED);
					sleep(1);
					failed_request_reset(_vect_index, partner_index);
				}

				//Potential partner has accepted request
				else if (status == partner_index)
				{
					set_connection(_vect_index, partner_index);
					send_buffer(_uid, only_code, strlen(only_code), false, CONN_MADE);
					matched = true;
				}

				//Request has timed out
				else if (status == -2)
				{
					send_buffer(_uid, only_code, strlen(only_code), false, TIMEOUT);
					sleep(1);
					failed_request_reset(_vect_index, partner_index);
				}

			}
		}

		//Potential partner name not found
		else
		{
			send_buffer(_uid, only_code, strlen(only_code), false, PARTNER_DNE);
		}

	}

	return matched;
}

void User::enter_name()
{
	char only_code[BUFFER_SIZE];
	char name[BUFFER_SIZE];
	bool valid_name = true;

	do
	{
		valid_name = true;
		memset(only_code, 0, BUFFER_SIZE);
		send_buffer(_uid, only_code, strlen(only_code), false, UNAME_REQUEST);
		
		memset(name, 0, sizeof name);
		receive(_uid, name, BUFFER_SIZE);
		//No other user has this name
		if (name_in_set(name, _uid) == -1)
		{
		
			add_to_set(name, _vect_index, _uid);
			memset(only_code, 0, BUFFER_SIZE);
			send_buffer(_uid, only_code, strlen(only_code), false, UNAME_VALID);
		}

		//Other user has this name
		else
		{
			memset(only_code, 0, BUFFER_SIZE);
			send_buffer(_uid, only_code, strlen(only_code), false, UNAME_TAKEN);
			sleep(1);
			valid_name = false;
		}

	} while(!valid_name);



	strncpy(_name, name, strlen(name));

	return;
}

char* User::get_name()
{
	return _name;
}

void User::set_vect_index(int vi)
{
	_vect_index = vi;
}

int User::get_vect_index()
{
	return _vect_index;
}

void User::handle_messages()
{
	int status;

	int partner_sock = get_partner_sock(_vect_index);
	int max_fd;
	struct timespec tv;
	fd_set fds;
	fd_set temp_fds;
	std::vector<int> fds_vect = { _uid, partner_sock };
	char message[BUFFER_SIZE];

	max_fd = init_fd_set(fds_vect, SELECT_TIMEOUT_S, SELECT_TIMEOUT_NS, fds, tv);

	while (1)
	{


		//wait for user to send or receive message
		do
		{
			temp_fds = fds;
			status = pselect(max_fd + 1, &temp_fds, NULL, NULL, &tv, NULL);
		} while ((select == 0) && (connection_active(_vect_index)));

		//User has sent message
		if (FD_ISSET(_uid, &temp_fds))
		{
			//write user's message into message buffer
			receive(_uid, message, BUFFER_SIZE);

			//Send contents of message buffer to partner
			send_buffer(partner_sock, message, strlen(message), true);
		}

		//Partner has sent message
		else if (FD_ISSET(partner_sock, &temp_fds))
		{
			//Write partner's message into message buffer
			receive(partner_sock, message, BUFFER_SIZE);
			printf("%s\n", message);
			//Send contenets of message buffer to partner
			send_buffer(_uid, message, strlen(message), true);
		}

		//handle lost connection
		else if (!connection_active(_vect_index))
		{
			//condition to loop back to ask
			status = 0;
		}
	}

	return;
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
						sleep(0.5);
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

				
	}
	//printf("Ending State: %d\n", _state);
}
