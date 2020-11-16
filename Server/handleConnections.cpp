#include "handleConnections.h"
#include "User.h"
#include "Constants.h"
#include "wait.h"
#include "sharedInfo.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <mutex>
#include <iostream>
#include <vector>



std::mutex index_mtx;
int userIndex = 0;


void initUserIndex(User& user)
{
    //lock released when out of scope
    std::lock_guard<std::mutex> lock(index_mtx);

    user.set_vect_index(userIndex);
    userIndex++;
}


void manageConnection(void* arg)
{
    static int user_count = 0;
    int client_fd = *((int*)arg);
    int valread;
    char buffer[BUFFER_SIZE] = { 0 };
    fd_set fds;
    struct timespec tv;
    std::vector<int> fds_vect;

    //Initialize file descriptor set this thread will be checking on
    fds_vect.push_back(client_fd);
    init_fd_set(fds_vect, SELECT_TIMEOUT_S, SELECT_TIMEOUT_NS, fds, tv);
    fds_vect.clear();
    
    /****Initialize thisUser****/
    User thisUser(client_fd);
    if (user_count < MAX_THREADS)
    {
        initUserIndex(thisUser);
        thisUser.enter_name();
        user_count++;
    }

    //add in error message for too many users
    /*else
    {
        send()
    }*/

    printf("%s is connected, client_fd id %d\n", thisUser.get_name(), client_fd);

    //Prevent back to back messages being sent together
    sleep(1);

    //Wait until matched with messaging partner
    while(!thisUser.select_user(fds, tv));
    
    //Main messaging loop
    thisUser.handle_messages();


}





