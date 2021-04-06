#pragma once
#include <vector>
#include<sys/select.h>
#include <string>

class User {

	private:
		int _uid;
		int _connected_uid;
		char _name[1024];
		int _state;

	public:
		User();
		User(int socket_fd);
		char* get_name();
		int get_socket();
		int get_partner_socket();
		void set_partner_socket(int p);
		int get_state();
		void set_state(int state);
		bool handleIncoming();
		void send_packet(uint16_t code_mask, std::string additional_info, bool to_self);
};
