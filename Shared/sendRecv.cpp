#include "sendRecv.h"
#include "Constants.h"
#include "concatChars.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>


void send_buffer(int sock_fd, bool from_user)
{
    char message[BUFFER_SIZE];
    memset(message, 0, sizeof message);
    std::cin >> message;
    sent_from(from_user, message);
    send(sock_fd, message, strlen(message), 0);
}

void send_buffer(int sock_fd, char *message, int size, bool from_user, int code)
{
    char codes[13] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    char c[1] = { codes[code] };
    char to_send[1024];
    concatChars(to_send, message, c);

    sent_from(from_user, to_send);
    send(sock_fd, to_send, strlen(to_send), 0);

    
}
void send_buffer(int sock_fd, char* message, int size, bool from_user)
{
    sent_from(from_user, message);
    send(sock_fd, message, strlen(message), 0);
}



void receive(int sock_fd) //receives return bools as well
{
    char message[BUFFER_SIZE];
    memset(message, 0, sizeof message);
    read(sock_fd, message, 1024);
    remove_sent_from(message);
    printf("%s\n", message);
}

bool receive(int sock_fd, char message[], int size)
{
    bool from_user = true;
    memset(message, 0, size);
    read(sock_fd, message, 1024);
    if(message[strlen(message) - 1] == 'S')
    {
      from_user = false;
    }
    remove_sent_from(message);
    return from_user;
}

void sent_from(bool T_user_F_server, char* message)
{
  if(T_user_F_server) //User is sending
  {
    //Make the message output "U" after itself
    strcat(message, "U");
  }
  else //Server is sending
  {
    //Make the message output "S" after itself
    strcat(message, "S");
  }
}

//function to undo sent from
void remove_sent_from(char* message)
{
  message[strlen(message) - 1] = '\0';
}
