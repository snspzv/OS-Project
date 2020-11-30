#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <thread>
#include <sys/time.h>
#include <sys/select.h>
#include "handleConnections.h"
#include "Constants.h"

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread; //Integers representing the server file descriptor, and a new socket
    struct sockaddr_in address; //Represents the server address and port address
    int opt = 1; //An integer used when setting the socket options
    int addrlen = sizeof(address); //An integer for the size of the address
    char buffer[1024] = {0}; //character array of all zeros the size of a socket buffer
    fd_set readfds; // set of socket descriptors


    // Creating socket file descriptor using IP and TCP (sock_stream with 3rd option 0 defaults to TCP)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // SOL_SOCKET - socket at API level
    // SO_REUSEADDR and SO_REUSEPORT - allows address and port to have multiple sockets
    // optval - enable given options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET; //IPv4
    address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    address.sin_port = htons( PORT ); //host to network short

    // Bind to port 8080
    //Second parameter casts address to protocol independent sockaddr type
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    //Will queue 10 pending connections
    if (listen(server_fd, BACKLOG) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::thread threadArray[BACKLOG + 5];
    int i = 0;

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr*) & address, (socklen_t*) & addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        //Creates a new thread for each new client that connects to the server 
        threadArray[i] = std::thread(manageConnection, &new_socket);
        i++;
        if (i >= 50)
        {
            i = 0;
            while (i < 50)
            {
                threadArray[i].join();
            }
            i = 0;
        }

    }
    return 0;
}
