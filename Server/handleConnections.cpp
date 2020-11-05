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
    int new_socket = *((int*)arg);
    int valread;
    User* p_user;
    char buffer[BUFFER_SIZE] = { 0 };
    char temp_name[BUFFER_SIZE] = "Wobbe";

    if (user_count < MAX_THREADS)
    {
        //To do: place locks around access to vector and assigning pointer
        user_vector.push_back(User(new_socket, temp_name));
        p_user = &user_vector.back();
        //Lock ends here


        user_count++;
    }

    while(!(p_user->select_user(new_socket, user_vector)));
    {
      //Waits until User to message is found
    }

    //std::cout << client_admin;
    //Process contents of from_client here
    //select user to talk to if exists
    //request again if not
    //Should be done in User class function
    //Current user is finding if the person they want to talk to is connected to the server


    while (1)
    {
        std::cout << new_socket << std::endl;

        valread = read(new_socket, buffer, BUFFER_SIZE);
        if (buffer)
            send(new_socket, buffer, strlen(buffer), 0);

    }


}
