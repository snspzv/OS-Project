#pragma once

void send(int sock_fd);

void send(int sock_fd, char message[], int size);

void receive(int sock_fd);

void receive(int sock_fd, char message[], int size);