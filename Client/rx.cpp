#include "rx.h"
#include "Constants.h"
#include "sendRecv.h"
#include "concatChars.h"
#include "Output.h"
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void cp_partner_name(char dest[], char src[])
{
	for (int i = 0; i < strlen(src) - 1; i++)
	{
		dest[i] = src[i];
	}

	dest[strlen(src) - 1] = 0;
}

void print_sm(int index)
{
	extern const char servMessages[20][BUFFER_SIZE];
	printf("%s\n", servMessages[index - 1]);
	return;
}

void print_sm(int index, char prepend[])
{
	extern char servMessages[20][BUFFER_SIZE];
	char for_cat[BUFFER_SIZE];
	concatChars(for_cat, prepend, servMessages[index -1]);
	printf("%s\n", for_cat);
	return;
}

int incoming(int sock_fd, char partner_name[], int log_fd, bool & tx_user_message, bool & entering_name)
{
	char message[BUFFER_SIZE];
	extern const char servMessages[20][BUFFER_SIZE];
	
	tx_user_message = false;

	//Server Message
	if (!receive(sock_fd, message, BUFFER_SIZE))
	{
		//Evaluate code received
		switch ((message[strlen(message) - 1])) {
			
			case UNAME_REQUEST:
				print_sm(UNAME_REQUEST);
				return TX_NEXT;

			case UNAME_TAKEN:
				print_sm(UNAME_TAKEN);
				return RX_NEXT;

			case UNAME_VALID:
				print_sm(UNAME_VALID);
				return RX_NEXT;

			case GET_PARTNER:
				print_sm(GET_PARTNER);
				entering_name = true;
				return EITHER_NEXT;

			case PARTNER_HAS_REQUESTED:
				cp_partner_name(partner_name, message);
				print_sm(PARTNER_HAS_REQUESTED, partner_name);
				tx_user_message = false;
				return TX_NEXT;

			case PARTNER_DNE:
				print_sm(PARTNER_DNE);
				return RX_NEXT;

			case CONN_DENIED:
				print_sm(CONN_DENIED);
				return RX_NEXT;

			case CONN_MADE:
				print_sm(CONN_MADE);
				system("cmd.exe /c start cmd.exe /c wsl.exe tail -F messages.log");
				tx_user_message = true;
				return EITHER_NEXT;

			case NOT_AVAIL:
				print_sm(NOT_AVAIL);
				return RX_NEXT;

			case TIMEOUT:
				print_sm(TIMEOUT);
				return RX_NEXT;

			case BAD_RESPONSE:
				print_sm(BAD_RESPONSE);
				return EITHER_NEXT;

			case PARTNER_FOUND:
				print_sm(PARTNER_FOUND, partner_name);
				return RX_NEXT;
		}

	}

	//User message
	else if ((message[strlen(message) - 1]) == 'U')
	{
		//printf("TEST\n");
		//Read message from sock_fd and write to message buffer
		//receive(sock_fd, message, sizeof message);
		tx_user_message = true;
		//Output to log file
		write_to_log(message, log_fd, partner_name, true);

		//flush buffer associated with message_fd to ensure that contents are immediately written to messages.log
		fdatasync(log_fd);

		return EITHER_NEXT;
	}


	return 0;
}
