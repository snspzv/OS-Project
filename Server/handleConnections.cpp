#include "handleConnections.h"
#include "User.h"
#include "Constants.h"
#include "wait.h"
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
    fd_set fds;
    struct timespec tv;
    std::vector<int> fds_vect;

    fds_vect.push_back(client_fd);
    //fds can now be used with select, currently only checking client_fd
    init_fd_set(fds_vect, SELECT_TIMEOUT_S, SELECT_TIMEOUT_NS, fds, tv);
    fds_vect.clear();

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
    sleep(3);
    //Asking for messaging partner
    while(!(p_user->select_user(client_fd, user_vector, fds, tv)));
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
    int select_ret;
    // timeout structure passed into select
    struct timeval tv;

    //Timeout after 5 seconds
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
    do
    {
        select_ret = select(max_fd + 1, &fds_to_watch, NULL, NULL, &tv);
    } while (select_ret == 0);

        // return 0 if STDIN is not ready to be read.
        //return FD_ISSET(client_fd, &fds);
        return 1;
}
