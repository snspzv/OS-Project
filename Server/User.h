#pragma once
#include <vector>
#include<sys/select.h>

class User {

	private:
		int _uid;
		int _connected_uid;
		int _vect_index;
		char _name[1024];
		bool _connection_requested;
		int _request_accepted;

	public:
		User();
		User(int socket_fd);
		bool select_user(fd_set & fds, timespec & tv);
		void enter_name();
		char* get_name();
		void set_vect_index(int vi);
		int get_vect_index();
		void handle_messages();

};
