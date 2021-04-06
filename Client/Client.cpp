#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include "Constants.h"
#include "Output.h"
#include "rx.h"
#include "tx.h"
#include "cli_sock.h"

std::string partner_name;
std::string name;

int main(int argc, char const *argv[])
{
    //***************************
    //Try to connect with server
    //***************************
    int sock_fd;
    while(true)
    {
        if((sock_fd =  sock_init()) == -1)
        {
            std::string retry;
            std::cout << "Retry connecting to the server? (y/n): " << std::flush;
            std::cin >> retry;
            if((retry.length() == 1) && (retry[0] == 'n'))
            {
                return sock_fd;
            }
        }

        else
            break;
    }


    fd_set fds;
    fd_set temp_fds;
    FD_ZERO(&fds);
    FD_SET(sock_fd, &fds);
    FD_SET(STDIN_FILENO, &fds);
    int max_fd = sock_fd;
    int next = RX_NEXT;
    bool tx_user_message = false;
    bool entering_name = false;
    std::cout << CLR_SCR;

    //*************************************************
    //Main loop, waits on STDIN and socket with server
    //*************************************************
    while (true)
    {
        temp_fds = fds;
        int status = pselect(max_fd + 1, &temp_fds, NULL, NULL, NULL, NULL);
        std::cout << CLR_SCR;

        //User input
        if (FD_ISSET(STDIN_FILENO, &temp_fds))
        {
            outgoing(sock_fd, tx_user_message, entering_name);
        }

        //Incoming message
        else if (FD_ISSET(sock_fd, &temp_fds))
        {
            incoming(sock_fd, tx_user_message, entering_name);
        }
    }
    return 0;
}
