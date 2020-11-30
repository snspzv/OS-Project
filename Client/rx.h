#pragma once

//A function which takes the socket file descriptor, partner's name,
//the log file descriptor, if the user tranmitted the message, and
//if the user is entering a name
//Returns an integer deciding if the client will Recieve, Transmit or do Either next
int incoming(int sock_fd, char partner_name[], int log_fd, bool & tx_user_message, bool & entering_name);

//A funtion which prints a message up to a given index in a character array
void print_sm(int index);

//A function which appends a message to the beginning of a server
//message up to an index of the server message then prints the concatenated message 
void print_sm(int index, char prepend[]);
