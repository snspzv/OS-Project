#include "handleConnections.h"
#include "User.h"
#include "Constants.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <mutex>
#include <iostream>
#include <vector>



std::mutex mtx;
std::vector<User> user_vector;
std::vector<User> *p_vector = &user_vector;

void manageConnection(void* arg)
{
    static int user_count = 0;
    int client_fd = *((int*)arg);
    int valread;
    User* p_user;
    char buffer[BUFFER_SIZE] = { 0 };

    if (user_count < MAX_THREADS)
    {
        //To do: place locks around access to vector and assigning pointer
        user_vector.push_back(User(client_fd));
        p_user = &user_vector.back();
        p_user->enter_name(client_fd, user_vector);
        //Lock ends here


        user_count++;
    }

    printf("%s is connected, client_fd id %d\n", p_user->get_name(), client_fd);

    //Asking for messaging partner
    while(!(p_user->select_user(client_fd, user_vector)));
    {
      //Waits until User to message is found
    }




    while (1)
    {


        valread = read(client_fd, buffer, BUFFER_SIZE);
        if (buffer)
            send(client_fd, buffer, strlen(buffer), 0);

    }


}

int wait_recv_or_send(std::vector<int> fds)
{
    int max_fd = 0;
    // timeout structure passed into select
    struct timeval tv;
    //Timeout after 2 seconds
    tv.tv_sec = 5;
    
    // fd_set passed into select
    fd_set fds_to_watch;
    

    // Zero out the fds_to_watch
    FD_ZERO(&fds_to_watch);
    
    //Add fds to fds_to_watch
    for (auto& fd : fds)
    {
        FD_SET(fd, &fds_to_watch);
        
        if (fd > max_fd)
        {
            max_fd = fd;
        }
    }
    
    
    //wait on fds in fds_to_watch
    return select(max_fd + 1, &fds_to_watch, NULL, NULL, &tv);
    // return 0 if STDIN is not ready to be read.
    //return FD_ISSET(client_fd, &fds);
}
