#include "tx.h"
#include "Constants.h"
#include "sendRecv.h"
#include "Output.h"
#include <cstring>
#include <unistd.h>
#include <stdio.h>

int outgoing(int sock_fd, bool tx_user_message, char *name, int log_fd, bool & entering_name, char * partner_name)
{
	char message[1024];
	memset(message, 0, 1024);
	int ret;

		//Read message from STDIN and write to message buffer
		receive(STDIN_FILENO, message, sizeof message);
		
		
		//Checking if name has not been sent, means this time name has been sent
		if (name[0] == 0)
		{
			strncpy(name, message, strlen(message));
			ret =  RX_NEXT;
		}

		else if (entering_name)
		{
			memset(partner_name, 0, sizeof partner_name);
			strncpy(partner_name, message, strlen(message));
			entering_name = false;
			ret =  RX_NEXT;
		}

		else if(tx_user_message)
		{
			//Write contents of message buffer to log
			write_to_log(message, log_fd, name, false);
			ret = EITHER_NEXT;
		}


		//Send contents of message buffer to server
		send_buffer(sock_fd, message, strlen(message), true);

		return ret;
}
