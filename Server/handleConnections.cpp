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
    char temp_name[BUFFER_SIZE] = "Wobbe";

    if (user_count < MAX_THREADS)
    {
        //Cade: function for user to enter their name, must be a unique name

        //To do: place locks around access to vector and assigning pointer
        user_vector.push_back(User(client_fd, temp_name));
        p_user = &user_vector.back();
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
