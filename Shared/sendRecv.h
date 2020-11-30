#pragma once

//Sends a message which the client will enter themselves
void send_buffer(int sock_fd, bool from_user);

//Sends one of the Precoded messages from the server to a client
void send_buffer(int sock_fd, char * message, int size, bool from_user, int code);

//Recieves a message from a socket
void receive(int sock_fd);

//Recieves a message from a socket
//Returns true if the message is from a client
//Returns false if the message is from the server
bool receive(int sock_fd, char message[], int size);

//Sends a non Precoded message that is defined elsewhere in the code *useful when testing
void send_buffer(int sock_fd, char* message, int size, bool from_user);

//Attaches a character to the end of a message to describe if it is from the server or from the client
//Useful for debugging
void sent_from(bool T_user_F_server, char *message);

//Removes the character at the end of a message
//Cleans up the output when not debugging
void remove_sent_from(char* message);
