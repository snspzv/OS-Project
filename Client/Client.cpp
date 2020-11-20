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


int main(int argc, char const *argv[])
{
    
    char message[1024];
    char name[1024];
    char server_message[1024];
    int sock_fd = 0, valread, message_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    int max_fd;
    std::vector<int> fds_vect;
    fd_set fds;
    fd_set temp_fds;
    int status;
    struct timespec tv;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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

    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    fds_vect.push_back(sock_fd);
    fds_vect.push_back(STDIN_FILENO);
    max_fd = init_fd_set(fds_vect, SELECT_TIMEOUT_S, SELECT_TIMEOUT_NS, fds, tv);

    //Open log file in write only mode, truncate to 0 bytes if exists, and create if it does not exist
    message_fd = open("./messages.log", O_WRONLY | O_TRUNC | O_CREAT);
    strcpy(message, "This is a test message of  over 50 characters vsdakn vsdakjn testing testing teting akndf dsk dlk  geskklsdf lkfads lkndsf;akn fs kldsfn nkl");
    strcpy(name, "SAM");
    
    write_to_log(message, message_fd, name, true);
    write_to_log(message, message_fd, name, false);
    //Wait until server gives ok on username
    do
    {
        //Asking for username
        receive(sock_fd);
        //Sending username
        send_buffer(sock_fd, true);
        //Receive feedback on uniqueness of username
        receive(sock_fd, server_message, sizeof server_message);
        printf("%s\n", server_message);

    } while (server_message[0] == 'T');

    //get messaging partner set up
    do
    {
        //Send user to talk to OR wait for other user to connect
        receive(sock_fd);

        //wait on STDIN or socket with no timeout
        temp_fds = fds;
        status = pselect(max_fd + 1, &temp_fds, NULL, NULL, NULL, NULL);

        //User has entered name of potential messaging partner
        if (FD_ISSET(STDIN_FILENO, &temp_fds))
        {
            //Read name from STDIN and write to message buffer
            receive(STDIN_FILENO, message, sizeof message);

            //Send name in message buffer to server
            send_buffer(sock_fd, message, strlen(message), true);

            receive(sock_fd);

            //Server notifies if connection is made or not
            receive(sock_fd);
        }

        //Other user wants to become messaging partner
        else if (FD_ISSET(sock_fd, &temp_fds))
        {
            receive(sock_fd);

            temp_fds = fds;
            status = pselect(max_fd + 1, &temp_fds, NULL, NULL, NULL, NULL);
            //User has entered message to be sent
            if (FD_ISSET(STDIN_FILENO, &temp_fds))
            {
                //Read message from STDIN and write to message buffer
                receive(STDIN_FILENO, message, sizeof message);

                //Send name in message buffer to server
                send_buffer(sock_fd, message, strlen(message), true);
            }

            else if (FD_ISSET(sock_fd, &temp_fds))
            {
                //Read message from sock_fd and write to STDOUT
                receive(sock_fd);
            }
            //send_buffer(sock_fd, true);
        }



    } while (false);

        
    //Open new window to show conversation
    system("./new_window.sh");
    int wr;

    while(1)
    {
        //wait on STDIN or socket with no timeout
        temp_fds = fds;
        status = pselect(max_fd + 1, &temp_fds, NULL, NULL, NULL, NULL);

        //User has entered message to be sent
        if (FD_ISSET(STDIN_FILENO, &temp_fds))
        {
            //Read message from STDIN and write to message buffer
            receive(STDIN_FILENO, message, sizeof message);

            //Send name in message buffer to server
            send_buffer(sock_fd, message, strlen(message), true);
        }

        else if (FD_ISSET(sock_fd, &temp_fds))
        {
            //Read message from sock_fd and write to message buffer
            receive(sock_fd, message, sizeof message);

            //write message buffer to messages.log
            wr = write(message_fd, message, strlen(message));

            //flush buffer associated with message_fd to ensure that contents are immediately written to messages.log
            fdatasync(message_fd);
        }
    }


    return 0;
}
