#pragma once
#include<vector>
void manageConnection(void * arg);

int wait_recv_or_send(std::vector<int> fds);
