#pragma once

void send_buffer(int sock_fd, bool from_user);

void send_buffer(int sock_fd, char * message, int size, bool from_user, int code);

void receive(int sock_fd);

bool receive(int sock_fd, char message[], int size);

void send_buffer(int sock_fd, char* message, int size, bool from_user);

void sent_from(bool T_user_F_server, char *message);

void remove_sent_from(char* message);
