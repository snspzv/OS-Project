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

void manageConnection(void* arg)
{
    static int user_count = 0;
    int new_socket = *((int*)arg);
    int valread;
    User* p_user;
    char buffer[BUFFER_SIZE] = { 0 };
    char from_server[BUFFER_SIZE] = "Enter the name of the user you'd like to message: ";
    char client_admin[BUFFER_SIZE];
    
    if (user_count < MAX_THREADS)
    {
        //To do: place locks around access to vector and assigning pointer
        user_vector.push_back(User(new_socket));
        p_user = &user_vector.back();
        //Lock ends here

        
        user_count++;
    }

    send(new_socket, from_server, strlen(from_server), 0);
    read(new_socket, client_admin, BUFFER_SIZE);
    std::cout << client_admin;
    //Process contents of from_client here
    //select user to talk to if exists
    //request again if not
    //Should be done in User class function

    while (1)
    {
        std::cout << new_socket << std::endl;
        
        valread = read(new_socket, buffer, BUFFER_SIZE);
        if (buffer)
            send(new_socket, buffer, strlen(buffer), 0);
       
    }
    
    
}
