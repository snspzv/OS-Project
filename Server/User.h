#pragma once
#include <vector>
#include<sys/select.h>

class User {

	private:
		int _uid;
		int _connected_uid;
		char _name[1024];
		int _state;
		int _request_uid;

	public:
		User();
		User(int socket_fd);
		char* get_name();
		int get_socket();
		int get_partner_socket();
		void set_partner_socket(int p);
		void set_state(int state);
		int get_state();
		void handleIncoming();
};
