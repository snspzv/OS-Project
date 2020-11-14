#pragma once
#include <vector>
#include<sys/select.h>

class User {

	private:
		int _uid;
		int _connected_uid;
		char _name[1024];
		bool _connection_requested;

	public:
		User();
		User(int socket_fd);
		bool select_user(int new_socket, std::vector<User> p_vector, fd_set & fds, timespec & tv);
		void enter_name(int new_socket, std::vector<User> p_vector);
		char* get_name();
};
