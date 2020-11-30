#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include "wait.h"
#include "sendRecv.h"
#include "Constants.h"
#include "Output.h"
#include "rx.h"
#include "tx.h"


int main(int argc, char const *argv[])
{

    char message[1024];   //character array that will be sent through sockets
    char name[1024];      //character array representing the name of a client
    char partner_name[1024];  //character array representing the name of a client's messaging partner
    char server_message[1024]; //character array of a message from the server
    int sock_fd = 0, valread, message_fd; //Integers representing socket file descriptor, value to be read, and message file descriptor
    struct sockaddr_in serv_addr; //A struct for the client address and port address
    char buffer[1024] = {0}; //character array of all zeros the size of a socket buffer
    int max_fd; //the maximum file descriptor
    std::vector<int> fds_vect;  //A vector of file descriptor Integers
    fd_set fds; //A variable which sets the file descriptor
    fd_set temp_fds; //A variable for a temporary file descriptor
    int status; //A variable to keep track of the the status of the socket
    struct timespec tv; //A varible holding a time interval in seconds and nanoseconds
    bool no_partner = true; //boolean value that keeps track of if the client has a partner or not

    //Gives an error if the Socket is not created correctly
    //This is currently unused but would be necessary if the user was involved in creating the socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    //Defines the address and port for the server to use
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    //Returns an error if the conection to the server fails
    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    //Prepares the initial socket and max socket file descriptors
    fds_vect.push_back(sock_fd);
    fds_vect.push_back(STDIN_FILENO);
    max_fd = init_fd_set(fds_vect, SELECT_TIMEOUT_S, SELECT_TIMEOUT_NS, fds, tv);

    //Open log file in write only mode, truncate to 0 bytes if exists, and create if it does not exist
    message_fd = open("./messages.log", O_WRONLY | O_TRUNC | O_CREAT);

    //Wait until server gives ok on username
    int next = RX_NEXT; //An integer which controls if the client will transmit or recieve a message next
    bool tx_user_message = false; //boolean to keep track of if a user transmitted a message
    bool entering_name = false; //boolean to track if the client is entering their name
    memset(name, 0, BUFFER_SIZE); //CLears the meemory of the character array name

    //The main loop of the client's program
    //Loops through transmitting or receiving messages
    //or the ability to do either when connected to another user
    while (true)
    {
        while (next == RX_NEXT)
        {
            //Recieves an incoming message then sets the value of next
            next = incoming(sock_fd, partner_name, message_fd, tx_user_message, entering_name);
        }

        while (next == TX_NEXT)
        {
            //Transmits an outgoing message then sets the value of next
            next = outgoing(sock_fd, tx_user_message, name, message_fd, entering_name, partner_name);
        }

        if(next == EITHER_NEXT)
        {
            //Checks the status of the current socket adress
            //Depending on the status decides to transmit or receive a message
            temp_fds = fds;
            status = pselect(max_fd + 1, &temp_fds, NULL, NULL, NULL, NULL);

            //User input
            if (FD_ISSET(STDIN_FILENO, &temp_fds))
            {
                next = TX_NEXT;
            }

            //Incoming message
            else if (FD_ISSET(sock_fd, &temp_fds))
            {
                next = RX_NEXT;
            }
        }
    }

    return 0;
}
