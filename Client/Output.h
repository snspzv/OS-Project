#pragma once

//A function which takes a message, the socket file descriptor of that message,
//the client name, and whether or not the message is from itself and then writes to
//a log for debugging purposes
void write_to_log(char* message, int message_fd, char* name, bool from_self);
