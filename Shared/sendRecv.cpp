#include "sendRecv.h"
#include "Constants.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>


void send(int sock_fd)
{
    char message[BUFFER_SIZE];
    memset(message, 0, sizeof message);
    std::cin >> message;
    //printf("%s\n", message);
    send(sock_fd, message, strlen(message), 0);
}

void send(int sock_fd, char *message, int size)
{
    send(sock_fd, message, size, 0);
}

void receive(int sock_fd)
{
    char message[BUFFER_SIZE];
    memset(message, 0, sizeof message);
    read(sock_fd, message, 1024);
    printf("%s\n", message);
}

void receive(int sock_fd, char message[], int size)
{
    memset(message, 0, size);
    read(sock_fd, message, 1024);
}
