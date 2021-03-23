#pragma once
#include <sys/select.h>
#include <vector>
int init_fd_set(const std::vector<int> fds_vect, int timeout_s, int timeout_ms, fd_set & fds_p, timespec & tv);

int add_to_fd_set(const std::vector<int> fds_vect, fd_set& fd_p);