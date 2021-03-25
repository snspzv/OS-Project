#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <thread>
#include <sys/time.h>
#include <sys/select.h>
#include "Constants.h"
#include "wait.h"
#include <map>
#include <iterator>
#include <algorithm>
#include "User.h"
#include <mutex>
std::map<int, User> users;


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
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT |SO_KEEPALIVE, &opt, sizeof(opt)) < 0)
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

    fd_set fds;
    //struct timespec tv;
    //std::vector<int> fds_vect = { server_fd };
    //int max_fd = init_fd_set(fds_vect, SELECT_TIMEOUT_S, SELECT_TIMEOUT_NS, fds, tv);
    FD_ZERO(&fds);
    FD_SET(server_fd, &fds);
    int max_fd = server_fd;
    int ready_count;

    while (1)
    {
        fd_set temp_fds = fds;
        std::vector<int> dead_sockets;
        ready_count = pselect(max_fd + 1, &temp_fds, NULL, NULL, NULL, NULL);
        printf("Ready: %d\n", ready_count);
        if (ready_count > 0)
        {
            for (auto &connection : users)
            {
                if (FD_ISSET(connection.first, &temp_fds))
                {
                    if(!connection.second.handleIncoming())
                    {
                        dead_sockets.push_back(connection.first);
                    }
                    ready_count--;
                }
                
                if (ready_count == 0)
                    break;
            }

            for(auto &dead: dead_sockets)
            {
                users.erase(dead);
                FD_CLR(dead, &fds);
                //Lazy, find better way
                if(max_fd == dead);
                    max_fd--;
            }
            
            dead_sockets.clear();

            if (FD_ISSET(server_fd, &temp_fds))
            {
                new_socket = accept(server_fd, (struct sockaddr*) & address, (socklen_t*)&addrlen);
                users[new_socket] = User(new_socket);
                users[new_socket].handleIncoming();
                FD_SET(new_socket, &fds);
                if(max_fd < new_socket)
                    max_fd = new_socket;
                ready_count--;
            }
            
            ready_count = 0;
        }
    }
    return 0;
}
