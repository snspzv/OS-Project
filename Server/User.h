#pragma once
#include <vector>
#include<sys/select.h>

class User {

	private:
		int _uid;	//The socket file descriptor for the client
		int _connected_uid;	//The socket file descriptor for the client's partner
		int _vect_index; //The index the client has in the shared information vector
		char _name[1024]; //The name of the client
		bool _connection_requested; //tracks if a connections was requested or not
		int _request_accepted; //tracks if a request was accepted or not

	public:
		//Constructs the user given no arguments
		User();

		//Constructs the user given a socket file descriptor
		//Sets the uid to the socket file descriptor value
		User(int socket_fd);

		//Function to select a user to partner with
		//Returns true if successful false if not
		bool select_user(fd_set & fds, timespec & tv);

		//Function to enter the client's name
		void enter_name();

		//Functions to return a clients name
		char* get_name();

		//Function to set the vector index of the client
		//Sets the vector index to vi
		void set_vect_index(int vi);

		//Function which gets the shared vector index for this client
		int get_vect_index();

		//Function to handle two users messaging back and forth with one another
		//can send or recieve messages
		void handle_messages();

};
