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



User::User(int socket_fd)
{
	_uid = socket_fd; //each new socket will have unique file descriptor
	_connected_uid = 0;
	//strncpy(_name, name, strlen(name));
	_connection_requested = false;
	_request_accepted = 0;
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
	char bad_name[BUFFER_SIZE] = "Invalid name. Try again\n";
	char successful_con[BUFFER_SIZE] = "Connection Successful.";
	char deny_con[BUFFER_SIZE] = "Connection Denied.";
	char busy[BUFFER_SIZE] = "This user is currently not available :(";
	char timeout_err[BUFFER_SIZE] = "Your request has timed out";
	char invalid_response[BUFFER_SIZE] = "Invalid response. Please answer 'y' or 'n'";
	char asking_partner[BUFFER_SIZE] = " found, asking them now";
	char name[BUFFER_SIZE];
	char yes_no[BUFFER_SIZE];
	int status;
	bool matched = false;
	bool request_ongoing = true;
	int bad_responses = 0;
	fd_set temp_fds;
	tv.tv_sec = 2;
	send(_uid, from_server, strlen(from_server));

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
		send(_uid, request_approval, strlen(request_approval));

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
				send(_uid, timeout_err, strlen(timeout_err));
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
					send(_uid, successful_con, strlen(successful_con));
					matched = true;
					request_ongoing = false;
				}

				//User denies partner
				else if ((yes_no[0] == 'N') || (yes_no[0] == 'n'))
				{
					deny_request(requester_index);
					send(_uid, deny_con, strlen(deny_con));
					request_ongoing = false;
				}

				//Invalid response
				else
				{
					send(_uid, invalid_response, strlen(invalid_response));
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
		read(_uid, name, BUFFER_SIZE);

		int partner_index = name_in_set(name, _uid);

		//Potential partner found
		if (partner_index != -1)
		{
			//Partner is busy and cannot be requested
			if (!request_partner(_vect_index, partner_index))
			{
				send(_uid, busy, strlen(busy));
			}

			//Partner available and request flag set
			else
			{
				char final_asking[BUFFER_SIZE];
				concatChars(final_asking, name, asking_partner);
				send(_uid, final_asking, strlen(final_asking));

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
					send(_uid, deny_con, strlen(successful_con));
					failed_request_reset(_vect_index, partner_index);
				}

				//Potential partner has accepted request
				else if (status == partner_index)
				{
					set_connection(_vect_index, partner_index);
					send(_uid, successful_con, strlen(successful_con));
					matched = true;
				}

				//Request has timed out
				else if (status == -2)
				{
					send(_uid, timeout_err, strlen(timeout_err));
					failed_request_reset(_vect_index, partner_index);
				}

			}
		}

		//Potential partner name not found
		else
		{
			send(_uid, bad_name, strlen(bad_name));
		}

	}
	
	return matched;
}

void User::enter_name()
{
	char enter_name[BUFFER_SIZE] = "Enter your username: ";
	char not_unique[BUFFER_SIZE] = "The username you entered is already taken. Try again.";
	char unique[BUFFER_SIZE] = "Valid username!";
	char name[BUFFER_SIZE];
	bool valid_name = true;

	do
	{
		valid_name = true;
		send(_uid, enter_name, strlen(enter_name), 0);
		memset(name, 0, sizeof name);
		read(_uid, name, BUFFER_SIZE);

		//No other user has this name
		if (name_in_set(name, _uid) == -1)
		{
			add_to_set(name, _vect_index, _uid);
			send(_uid, unique, strlen(unique), 0);
		}

		//Other user has this name
		else
		{
			send(_uid, not_unique, strlen(not_unique), 0);
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
			send(partner_sock, message, strlen(message));
		}

		//Partner has sent message
		else if (FD_ISSET(partner_sock, &temp_fds))
		{
			//Write partner's message into message buffer
			receive(partner_sock, message, BUFFER_SIZE);

			//Send contenets of message buffer to partner
			send(_uid, message, strlen(message));
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
