#include "rx.h"
#include "Constants.h"
#include "Output.h"
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <map>
#include <string>
#include <fstream>
#include <iostream>

int log_fd;
extern std::map<int, std::string> servMessages;
extern std::string partner_name;
std::string log_file;
std::fstream fs;

std::string bottom = "\n-------------------------------------------------------------------------------------------\n-> ";
std::string top = "\t\t\t\tTo \n-------------------------------------------------------------------------------------------\n";
void print_sm(int key)
{
	std::cout << servMessages[key] << std::endl;
	//printf("%s\n", servMessages[key].c_str());
}

void print_sm(int key, std::string prepend)
{
	std::cout << prepend << servMessages[key] << std::endl;
	//printf("%s\n", (prepend + servMessages[key]).c_str());
}

int incoming(int sock_fd, bool & tx_user_message, bool & entering_name)
{
	std::string header;
	std::string log_contents;
	char message[BUFFER_SIZE] = {};
	std::string command = "xterm -geometry 110 -e tail -F ";
	tx_user_message = false;
	read(sock_fd, message, BUFFER_SIZE);
	//printf("message[0]: %d\nmessage[1]%d\n", message[0], message[1]);
	header.append(message, 2);
	int state = -1;
	
	for(std::map<int,std::string>::reverse_iterator it = servMessages.rbegin(); it!=servMessages.rend(); ++it)
	{
		//Check first and second bytes for which flags are set
		if(((it->first & uint8_t(header[1])) == it->first) || ((it->first & uint16_t(uint8_t(header[0]) << 8)) == it->first))
		{
			//printf("Key: %d\nheader[1]: %d\nheader[0]: %d\n", it->first, header[1], header[0]);
			switch (it->first) {
				case UNAME_REQUEST:
					print_sm(UNAME_REQUEST);
					state =  TX_NEXT;
					break;

				case UNAME_TAKEN:
					print_sm(UNAME_TAKEN);
					state =  RX_NEXT;
					break;

				case UNAME_VALID:
					print_sm(UNAME_VALID);
					state =  RX_NEXT;
					break;

				case GET_PARTNER:
					print_sm(GET_PARTNER);
					entering_name = true;
					state =  EITHER_NEXT;
					break;

				case PARTNER_HAS_REQUESTED:
					partner_name = &message[2];
					print_sm(PARTNER_HAS_REQUESTED, partner_name);
					tx_user_message = false;
					entering_name = false;
					state = TX_NEXT;
					break;

				case PARTNER_DNE:
					print_sm(PARTNER_DNE);
					state =  RX_NEXT;
					break;

				case CONN_DENIED:
					print_sm(CONN_DENIED);
					state =  RX_NEXT;
					break;

				case CONN_MADE:
					print_sm(CONN_MADE);
					log_file = (partner_name + ".log");
					top.insert(7, partner_name);
					sleep(1);
					std::cout << "\033[2J\033[1;1H" << top  << bottom << std::flush;
					tx_user_message = true;
					state =  EITHER_NEXT;
					break;

				case NOT_AVAIL:
					print_sm(NOT_AVAIL);
					state =  RX_NEXT;
					break;

				case TIMEOUT:
					print_sm(TIMEOUT);
					state =  RX_NEXT;
					break;

				case BAD_RESPONSE:
					print_sm(BAD_RESPONSE);
					state =  EITHER_NEXT;
					break;

				case PARTNER_FOUND:
					print_sm(PARTNER_FOUND, partner_name);
					state =  RX_NEXT;
					break;

				case PARTNER_DISCONNECT:
					print_sm(PARTNER_DISCONNECT, partner_name);
					sleep(1);
					std::cout << "\033[2J\033[1;1H";
					partner_name.clear();
					state =  EITHER_NEXT;
					break;

				case USER_MESSAGE:
					tx_user_message = true;
					write_to_log(&message[2], log_fd, partner_name, false);
					fs.open(log_file, std::fstream::in);
					std::cout << top << fs.rdbuf() << bottom << std::flush;
					fs.close();
					state =  EITHER_NEXT;
					break;
			}
		}

	}

		return EITHER_NEXT;
}
