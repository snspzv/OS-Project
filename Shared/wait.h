#pragma once
#include <sys/select.h>
#include <vector>

//A function to initialize a set of file descriptors which will be watched by the server
//Returns an integer value of the maximum file descriptor
int init_fd_set(const std::vector<int> fds_vect, int timeout_s, int timeout_ms, fd_set & fds_p, timespec & tv);

//Adds a file descriptor to the set of file descriptors which will be watched
//Returns an integer value of the maximum file descriptor 
int add_to_fd_set(const std::vector<int> fds_vect, fd_set& fd_p);
