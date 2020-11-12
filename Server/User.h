#pragma once
#include <vector>

class User {

	private:
		int _uid;
		int _connected_uid;
		char _name[1024];

	public:
		User();
		User(int socket_fd, char * name);
		bool select_user(int new_socket, std::vector<User> p_vector);
		char* get_name();
};