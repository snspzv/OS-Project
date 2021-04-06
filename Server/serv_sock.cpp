#include "serv_sock.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/time.h>
#include <sys/select.h>
#include "Constants.h"
#include <map>
#include <iterator>
#include <algorithm>
#include "User.h"

extern std::map<int, User> users; 
static struct sockaddr_in address;
static int addrlen;

int sock_init()
{
    int server_fd;
     // Creating socket file descriptor using IP and TCP (sock_stream with 3rd option 0 defaults to TCP)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // SOL_SOCKET - socket at API level
    // SO_REUSEADDR and SO_REUSEPORT - allows address and port to have multiple sockets
    // SO_KEEPALIVE - send ACKS back and forth to watch for client exiting non-gracefully
    //Need to find time SO_KEEPALIVE uses
    int opt = 1; //enable given options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT |SO_KEEPALIVE, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    // Bind to port 8080
    //Second parameter casts address to protocol independent sockaddr type
    addrlen = sizeof(address); 
    address.sin_family = AF_INET; //IPv4
    address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    address.sin_port = htons( PORT ); //host to network short - gets correct endianess
    if (bind(server_fd, (struct sockaddr *)&address, addrlen)<0)
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

    return server_fd;
}

int new_connection(int server_fd)
{
    int new_socket = accept(server_fd, (struct sockaddr*) & address, (socklen_t*)&addrlen);
    users[new_socket] = User(new_socket);
    users[new_socket].handleIncoming();
    return new_socket;
}