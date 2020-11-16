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
    //printf("%s\n", message);
    sent_from(from_user, message);
    send(sock_fd, message, strlen(message), 0);
}

void send_buffer(int sock_fd, char *message, int size, bool from_user)
{
    sent_from(from_user, message);
    send(sock_fd, message, strlen(message), 0);
}

void receive(int sock_fd) //receives return bools as well
{
    char message[BUFFER_SIZE];
    memset(message, 0, sizeof message);
    read(sock_fd, message, 1024);
    printf("%s\n", message);
    //unconcat here
}

void receive(int sock_fd, char message[], int size)
{
    memset(message, 0, size);
    read(sock_fd, message, 1024);
    //unconcat here
}

void sent_from(bool T_user_F_server, char* message)
{
  char copy_of_message[BUFFER_SIZE];
  strncpy(copy_of_message, message, strlen(message));
  memset(message, 0, sizeof message);
  if(T_user_F_server) //User is sending
  {
    //Make the message output "U: " before itself
    char from_the_user[BUFFER_SIZE] = "U: ";
    concatChars(message, from_the_user, copy_of_message);
  }
  else //Server is sending
  {
    //Make the message output "S: " before itself
    char from_the_server[BUFFER_SIZE] = "S: ";
    concatChars(message, from_the_server, copy_of_message);
  }

  //write function to undo the concatenation of chars
}
