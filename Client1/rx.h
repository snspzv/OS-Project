#pragma once

int incoming(int sock_fd, char partner_name[], int log_fd, bool & tx_user_message, bool & entering_name);
void print_sm(int index);
void print_sm(int index, char prepend[]);