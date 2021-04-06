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
#include "serv_sock.h"
std::map<int, User> users;


int main(int argc, char const *argv[])
{
    char test[5];
    test[0] = 128;
    test[1] = 0;
    test[2] = 3;
    test[3] = 4;
    std::string t;
    t.append(test, 3);
    printf("size: %d\n", test[0]);
    int server_fd = sock_init();
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(server_fd, &fds);
    int max_fd = server_fd;
    int ready_count;

    while (1)
    {
        fd_set temp_fds = fds;
        std::vector<int> dead_sockets;
        printf("MAX_FD = %d\n", max_fd);
        ready_count = pselect(max_fd + 1, &temp_fds, NULL, NULL, NULL, NULL);

        if (ready_count > 0)
        {
            for (auto &connection : users)
            {
                if (FD_ISSET(connection.first, &temp_fds))
                {
                    if(!connection.second.handleIncoming())
                    {
                        printf("Socket died: %d\n", connection.first);
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
                //Not always right, find better way
                if(max_fd == dead)
                {
                    printf("Inclementing\n");
                    max_fd--;
                }
                    
            }
            
            dead_sockets.clear();

            if (FD_ISSET(server_fd, &temp_fds))
            {
                int new_socket = new_connection(server_fd);
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
