#pragma once

//A funtion that sends deals with an outgoing message
//Takes the socket file descriptor, if the user is transmitting,
//client name, log file descriptor, if the client is entering their name,
//and the name of the clients partner
//Returns an int which will determine if the user will Recieve, Transmit, or do Either next
int outgoing(int sock_fd, bool tx_user_message, char *name, int log_fd, bool & entering_name, char * partner_name);
