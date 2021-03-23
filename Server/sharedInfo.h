#pragma once
#include "Constants.h"

struct sharedInfo {
	int connection_requested;
	char name[BUFFER_SIZE];
	int connection_index;
	int sock_fd;
	bool busy;
	bool connected;
};

int name_in_set(char * check_name, int socket);

void add_to_set(char* name_in, int vect_index, int socket);

bool request_partner(int requester_index, int partner_index);

int check_request_status(int requester_index);

void failed_request_reset(int requester_index, int partner_index);

void set_not_busy(int vect_index);

bool is_not_busy(int vect_index);

int get_requester_name(int vect_index, char* requester_name);

void accept_request(int vect_index, int requester_index);

void deny_request(int requester_index);

void timeout_request(int requester_index);

void set_connection(int vect_index, int partner_index);

int get_partner_sock(int vect_index);

bool connection_active(int vect_index);
