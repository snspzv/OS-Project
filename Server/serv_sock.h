#pragma once
 #include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h> 

int sock_init();
int new_connection(int server_fd);