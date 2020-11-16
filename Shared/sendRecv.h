#pragma once

void send_buffer(int sock_fd, bool from_user);

void send_buffer(int sock_fd, char message[], int size, bool from_user);

void receive(int sock_fd);

void receive(int sock_fd, char message[], int size);

void sent_from(bool T_user_F_server, char *message);
