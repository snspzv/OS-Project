#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include "wait.h"
#include "sendRecv.h"
#include "Constants.h"


int main(int argc, char const *argv[])
{
    char message[1024];
    char server_message[1024];
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    int max_fd;
    std::vector<int> fds_vect;
    fd_set fds;
    struct timespec tv;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    fds_vect.push_back(sock);
    fds_vect.push_back(STDIN_FILENO);
    init_fd_set(fds_vect, SELECT_TIMEOUT_S, SELECT_TIMEOUT_NS, fds, tv);
    
    //Wait until server gives ok on username
    do
    {
        //Asking for username
        receive(sock);
        //Sending username
        send(sock);
        //Receive feedback on uniqueness of username
        receive(sock, server_message, sizeof server_message);
        printf("%s\n", server_message);
        
    } while (server_message[0] == 'T');
    
    
    //Send user to talk to OR wait for other user to connect
    receive(sock);
    send(sock);
   

    while(1)
    {
      
    }


    return 0;
}
